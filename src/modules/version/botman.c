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
 * @return JSON data of .version command.
 */
char *
version_botman_fetch(void *data, Eina_Bool *update)
{
   Module_Version *version;
   Module_Version_Element *mve;
   cJSON *json,
         *json_content;
   char *p = NULL;
   Eina_List *l;
   Gotham_Citizen *citizen;

   *update = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(data, NULL);

   version = data;
   citizen = version->gotham->me;

   json = cJSON_CreateObject();
   cJSON_AddStringToObject(json, "command", ".version");
   cJSON_AddStringToObject(json, "parameters", "");

   json_content = cJSON_CreateArray();
   EINA_LIST_FOREACH(version->versions.list, l, mve)
     {
        const char *s;
        char *version_item,
             *var;

        s = _version_botman_version_get(mve->cmd);
        if (!s)
          s = strdup("-- Not found --");

        /* Set local custom vars, prefixed with version_ */
        version_item = dupf("version_%s", mve->name);
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

        var = dupf("%s : %s", mve->name, s);
        if (var)
          cJSON_AddItemToArray(json_content, cJSON_CreateString(var));
        free(var);
        free((char *)s);
     }

   cJSON_AddStringToObject(json, "status", "ok");
   cJSON_AddItemToObject(json, "content", json_content);

   p = cJSON_Print(json);
   if (!p)
     ERR("Failed to serialize JSON");

   cJSON_Delete(json);
   return p;
}

/**
 * @brief Sends up to date .version command.
 * @param version Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
version_botman_command(Module_Version *version,
                       Gotham_Citizen_Command *command)
{
   char *s;
   Eina_Bool updated;

   EINA_SAFETY_ON_NULL_RETURN(version);
   EINA_SAFETY_ON_NULL_RETURN(command);

   s = version_botman_fetch(version, &updated);

   gotham_citizen_send(version->gotham->alfred, s);
   if (command->citizen != version->gotham->alfred)
     gotham_citizen_send(command->citizen, s);

   free(s);
   return;
}

Eina_Bool
version_botman_poll(void *data)
{
   Module_Version *version;
   char *s;
   Eina_Bool updated;

   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_TRUE);

   version = data;
   s = version_botman_fetch(version, &updated);
   if (!s)
     return EINA_TRUE;

   if (updated)
     {
        DBG("Sending new version info to alfred");
        gotham_citizen_send(version->gotham->alfred, s);
        version->sent_once = EINA_TRUE;
     }
   else
     DBG("No version change detected");
   free(s);
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
                              "version.conf.");
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
