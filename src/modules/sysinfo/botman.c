#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <cJSON.h>
#include <ctype.h>

#include "sysinfo.h"

/**
 * @addtogroup Gotham_Module_Sysinfo
 * @{
 */

/**
 * @brief Run command in a pipe and return command's result.
 * @param cmd Command to run
 * @return Command's result
 */
const char *
_botman_pipe_read(const char *cmd)
{
   Eina_Strbuf *buf = NULL;
   FILE *fd = NULL;
   char line[1024],
        *line_ptr = NULL;
   const char *ptr = NULL;

   DBG("cmd:“%s”", cmd);

   fd = popen(cmd, "r");
   if (!fd)
     {
        ERR("Error while running %s : %s", cmd, strerror(errno));
        goto func_end;
     }

   buf = eina_strbuf_new();

   while ((line_ptr = fgets(line, sizeof(line)-1, fd)))
     eina_strbuf_append(buf, line_ptr);

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);

func_end:
   if (fd) fclose(fd);

   return (ptr ? ptr : "-- Not found --");
}

/**
 * @brief Gather sysinfo (from sysinfo.conf).
 * Run through obj->hw hash values, syscall for each and store result
 * in custom vars (prefixed with sysinfo_)
 * @param obj Module object
 */
void
botman_sysinfo_get(Module_Sysinfo *obj)
{
   Gotham_Citizen *citizen;
   Eina_List *l;
   Module_Sysinfo_Conf_Item *item;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   citizen = obj->gotham->me;

   EINA_LIST_FOREACH(obj->conf->hw, l, item)
     {
        const char *value;
        char s[strlen(item->name)+8];
        Eina_Strbuf *buf;

        value = _botman_pipe_read(item->command);
        sprintf(s, "sysinfo_%s", item->name);

        buf = eina_strbuf_new();

        eina_strbuf_append(buf, value);

        eina_strbuf_trim(buf);
        eina_strbuf_replace_all(buf, "\n", "");

        DBG("var : %s=%s", s, eina_strbuf_string_get(buf));
        VARSET(s, "%s", eina_strbuf_string_get(buf));
        eina_strbuf_free(buf);
        free((char *)value);
     }
}

/**
 * @brief Return the sysinfo values to sender.
 * Run through obj->hw hash names, get the matching custom var
 * and send back results.
 * @param obj Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
botman_sysinfo_send(void *data,
                    Gotham_Citizen_Command *command)
{
   Module_Sysinfo *obj = data;
   Eina_Strbuf *buf;
   Gotham_Citizen *citizen;
   Module_Sysinfo_Conf_Item *item;
   Eina_List *l;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   citizen = obj->gotham->me;

   buf = eina_strbuf_new();

   EINA_LIST_FOREACH(obj->conf->hw, l, item)
     {
        const char *value;
        char s[strlen(item->name)+8];

        sprintf(s, "sysinfo_%s", item->name);

        value = VARGET(s);
        eina_strbuf_append_printf(buf, "%s : %s\n", item->name, value);
     }

   gotham_command_json_answer(".sysinfo", "", EINA_TRUE, buf,
                              obj->gotham, command->citizen, EINA_TRUE);
   eina_strbuf_free(buf);
}

/**
 * @brief Run a custom sys command through a pipe and return result.
 * @param obj Module object
 * @param command Gotham_Citizen_Command incoming command and citizen
 */
void
botman_sysinfo_command_run(void *data,
                           Gotham_Citizen_Command *command)
{
   Module_Sysinfo *obj = data;
   const char *value;
   Eina_Strbuf *buf;
   Module_Sysinfo_Conf_Item *item;
   Eina_Bool ret = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN(obj);

   buf = eina_strbuf_new();

   item = utils_item_find(obj->conf->commands, command->name);
   if (!item)
     {
        eina_strbuf_append_printf(buf, "Can't get command line "
                                       "for %s, aborting.",
                                       command->name);
        ERR("%s", eina_strbuf_string_get(buf));
        goto func_end;
     }

   value = _botman_pipe_read(item->command);
   eina_strbuf_append(buf, value);
   free((char *)value);
   ret = EINA_TRUE;

func_end:
   gotham_command_json_answer(command->name, "", ret, buf,
                              obj->gotham, command->citizen, EINA_FALSE);

   eina_strbuf_free(buf);

}

/**
 * @brief Add commands that can be run through botman.
 * Generic command .sysinfo will return cached informations (results of each
 * system command stored in obj->hw).
 * Specific commands (from obj->commands) will also be added.
 * @param obj Module object
 */
void
botman_commands_add(Module_Sysinfo *obj)
{
   Eina_List *l;
   Module_Sysinfo_Conf_Item *item;

   gotham_modules_command_add("sysinfo", ".sysinfo",
                              "[.sysinfo] - "
                              "This command allows you to get some system "
                              "informations (manufacturer, serial #, ...)",
                              botman_sysinfo_send);

   EINA_LIST_FOREACH(obj->conf->commands, l, item)
     {
        char desc[100];
        sprintf(desc, "[%s] - Run %s on server", item->name, item->command);
        DBG("%s", desc);
        gotham_modules_command_add("sysinfo", item->name, desc, botman_sysinfo_command_run);
     }
}


/**
 * @Brief Remove botman commands.
 * @param obj Module object.
 */
void
botman_commands_del(Module_Sysinfo *obj)
{
   Eina_List *l;
   Module_Sysinfo_Conf_Item *item;

   gotham_modules_command_del("sysinfo", ".sysinfo");

   EINA_LIST_FOREACH(obj->conf->commands, l, item)
     {
        gotham_modules_command_del("sysinfo", item->name);
     }
}


/**
 * @}
 */
