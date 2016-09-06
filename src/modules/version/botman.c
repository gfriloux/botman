#define _GNU_SOURCE
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cJSON.h>
#include <ctype.h>

#include "version.h"

/**
 * @addtogroup Gotham_Module_Version
 * @{
 */

/**
 * @brief Launch system call into a pipe.
 * Opens a pipe, exec the given command and returns the result
 * @param res char ** pointer to store result
 * @param cmd Command to execute
 * @param ... optional args to add
 * @returns EINA_TRUE on success, otherwise EINA_FALSE
 */
const char *
_version_botman_version_get(const char *cmd, ...)
{
   FILE *fd = NULL;
   char line[1024],
        *line_ptr = NULL,
        *cmd_out,
        *buffer,
        *s = NULL;
   Eina_Bool found = EINA_FALSE;
   Eina_Strbuf *buf = NULL;
   va_list argp;
   int len;

	va_start(argp, cmd);
	len = vasprintf(&buffer, cmd, argp);
	va_end(argp);

   if (len == -1)
     {
        ERR("Failed to format string");
        return NULL;
     }

   cmd_out = calloc(1, len + 6);
   if (!cmd_out)
     {
        ERR("Allocation failure");
        goto free_buffer;
     }
   strcpy(cmd_out, buffer);
   strcpy(cmd_out + len, " 2>&1");

   DBG("cmd:“%s”", cmd_out); /* Vulnerable to attack */

   buf = eina_strbuf_new();
   if (!buf)
     {
        ERR("Failed to allocate buffer");
        goto free_cmd_out;
     }


   fd = popen(cmd_out, "r");
   if (!fd)
     {
        ERR("Failed to execute command");
        goto found_str;
     }

   /*
    * Seems too simple to be bullet proof. Really.
    * Where are the error checks ?
    */
   while ((line_ptr = fgets(line, sizeof(line)-1, fd)))
     {
        unsigned int lastchar =  strlen(line_ptr)-1;
        if (line_ptr[lastchar] == '\n')
          line_ptr[lastchar] = '\0';

        eina_strbuf_append(buf, line_ptr);
        found = EINA_TRUE;
     }

found_str:
   if (!found)
     eina_strbuf_append(buf, "-- Not found --");

   s = eina_strbuf_string_steal(buf);

   pclose(fd);
   eina_strbuf_free(buf);
free_cmd_out:
   free(cmd_out);
free_buffer:
   free(buffer);
   return s;
}

/**
 * @brief Gather softwares version by running a system calls.
 * @param data Module_Version structure.
 */
void
version_botman_fetch(void *data,
                     Gotham_Citizen_Command *command,
                     Eina_Bool *update)
{
   Module_Version *version;
   Module_Version_Conf_Software *mvcs;
   Eina_List *l;
   Gotham_Citizen *citizen;
   Eina_Strbuf *buf;

   *update = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN(data);

   version = data;
   citizen = version->gotham->me;

   buf = eina_strbuf_new();

   EINA_LIST_FOREACH(version->conf->softwares, l, mvcs)
     {
        const char *s;
        char *version_item;

        s =  _version_botman_version_get(mvcs->command);
        if (!s) s = strdup("-- Not found --");

        version_item = dupf("version_%s", mvcs->name);
        if (version_item)
          {
             const char *old;

             old = VARGET(version_item);
             if ((!old) || (strcmp(old, s)))
               {
                  VARSET(version_item, "%s", s); /* Uses citizen !! */
                  *update = EINA_TRUE;
               }
          }
        free(version_item);
        eina_strbuf_append_printf(buf, "%s : %s\n", mvcs->name, s);

     }

   if ((!command->citizen) || (command->citizen == version->gotham->alfred))
     gotham_command_json_answer(".version", "", EINA_TRUE, buf, version->gotham,
                                version->gotham->alfred, EINA_FALSE);
   else
     gotham_command_json_answer(".version", "", EINA_TRUE, buf, version->gotham,
                                command->citizen, EINA_FALSE);
   eina_strbuf_free(buf);
}

/**
 * @brief Sends up to date .version command.
 * @param version Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
version_botman_command(void *data,
                       Gotham_Citizen_Command *command)
{
   Module_Version *version = data;
   Eina_Bool updated;

   EINA_SAFETY_ON_NULL_RETURN(version);
   EINA_SAFETY_ON_NULL_RETURN(command);

   version_botman_fetch(version, command, &updated);
   return;
}

Eina_Bool
version_botman_poll(void *data)
{
   Module_Version *version;
   Eina_Bool updated;

   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_TRUE);

   version = data;
   version_botman_fetch(version, NULL, &updated);

   if (updated) version->sent_once = EINA_TRUE;
   else DBG("No version change detected");
   return EINA_TRUE;
}

/**
 * @brief Register commands that Botman can handle.
 * Command list :<br />
 * .version : return software versions list.
 */
void
version_botman_commands_register(void)
{
   gotham_modules_command_add("version", ".version",
                              "[.version] - "
                              "This command returns the version of some "
                              "installed softwares. Software list and custom "
                              "version commands can be specified in "
                              "version.conf.",
                              version_botman_command);
}

/**
 * @brief Unregister Botman commands
 */
void
version_botman_commands_unregister(void)
{
   gotham_modules_command_del("version", ".version");
}

/**
 * @}
 */
