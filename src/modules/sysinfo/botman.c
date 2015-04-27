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
 * @brief Send answer in JSON format.
 * @param cmd Command requested
 * @param params Command parameters
 * @param status EINA_TRUE on success, EINA_FALSE otherwise
 * @param gotham Gotham structure
 * @param content String buffer containing message content
 * @param citizen Gotham_Citizen we reply to
 * @param send_to_alfred EINA_TRUE if message has to be also sent
          to Alfred, EINA_FALSE otherwise
 */
void
module_json_answer(const char *cmd,
                   const char *params,
                   Eina_Bool status,
                   Eina_Strbuf *content,
                   Gotham *gotham,
                   Gotham_Citizen *citizen,
                   Eina_Bool send_to_alfred)
{
   cJSON *json_obj,
         *json_content;
   char **split;
   unsigned int i;
   char *ptr;

   json_obj = cJSON_CreateObject();
   cJSON_AddStringToObject(json_obj, "command", cmd);
   cJSON_AddStringToObject(json_obj, "parameters", params);
   cJSON_AddStringToObject(json_obj, "status", status ? "ok" : "ko");

   json_content = cJSON_CreateArray();
   split = eina_str_split(eina_strbuf_string_get(content), "\n", 0);
   for (i=0; split[i]; i++)
     if (split[i][0])
       cJSON_AddItemToArray(json_content, cJSON_CreateString(split[i]));

   cJSON_AddItemToObject(json_obj, "content", json_content);
   if (split[0]) free(split[0]);
   free(split);

   ptr = cJSON_Print(json_obj);
   cJSON_Delete(json_obj);
   gotham_citizen_send(citizen, ptr);

   if (send_to_alfred && (strcmp(citizen->jid, gotham->alfred->jid)))
     gotham_citizen_send(gotham->alfred, ptr);

   free(ptr);
   return;
}

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
   Eina_Iterator *it;
   void *data;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   citizen = obj->gotham->me;

   /* Run through obj->hw (values to store in custom vars) */
   it = eina_hash_iterator_tuple_new(obj->hw);

   while(eina_iterator_next(it, &data))
     {
        Eina_Hash_Tuple *t = data;
        const char *value;
        const char *name = t->key,
                   *cmd = t->data;

        char item[strlen(name)+8];

        value = _botman_pipe_read(cmd);
        sprintf(item, "sysinfo_%s", name);

        DBG("var : %s=%s", item, value);
        VARSET(item, "%s", value);
        free((char *)value);
     }
   eina_iterator_free(it);
}

/**
 * @brief Return the sysinfo values to sender.
 * Run through obj->hw hash names, get the matching custom var
 * and send back results.
 * @param obj Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
botman_sysinfo_send(Module_Sysinfo *obj,
                    Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;
   Gotham_Citizen *citizen;
   Eina_Iterator *it;
   void *data;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   citizen = obj->gotham->me;

   buf = eina_strbuf_new();

   /* Run through obj->hw to get custom vars */
   it = eina_hash_iterator_key_new(obj->hw);

   while(eina_iterator_next(it, &data))
     {
        const char *value;
        const char *name = data;

        char item[strlen(name)+8];

        sprintf(item, "sysinfo_%s", name);

        value = VARGET(item);
        eina_strbuf_append_printf(buf, "%s : %s\n", name, value);
     }
   eina_iterator_free(it);

   module_json_answer(".sysinfo",
                      "",
                      EINA_TRUE,
                      buf,
                      obj->gotham,
                      command->citizen,
                      EINA_TRUE);

   eina_strbuf_free(buf);
}

/**
 * @brief Run a custom sys command through a pipe and return result.
 * @param obj Module object
 * @param command Gotham_Citizen_Command incoming command and citizen
 */
void
botman_sysinfo_command_run(Module_Sysinfo *obj,
                           Gotham_Citizen_Command *command)
{
   const char *cmd,
              *value;
   Eina_Strbuf *buf;
   Eina_Bool ret = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN(obj);

   buf = eina_strbuf_new();

   cmd = eina_hash_find(obj->commands, command->name);
   if (!cmd)
     {
        eina_strbuf_append_printf(buf, "Can't get command line "
                                       "for %s, aborting.",
                                       command->name);
        ERR("%s", eina_strbuf_string_get(buf));
        goto func_end;
     }

   value = _botman_pipe_read(cmd);
   eina_strbuf_append(buf, value);
   free((char *)value);
   ret = EINA_TRUE;

func_end:
   module_json_answer(command->name,
                      "",
                      ret,
                      buf,
                      obj->gotham,
                      command->citizen,
                      EINA_FALSE);

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
   Eina_Iterator *it;
   void *data;

   gotham_modules_command_add("sysinfo", ".sysinfo",
                              "[.sysinfo] - "
                              "This command allows you to get some system "
                              "informations (manufacturer, serial #, ...)");

   /* Run through obj->commands and add matching commands */
   it = eina_hash_iterator_tuple_new(obj->commands);

   while(eina_iterator_next(it, &data))
     {
        Eina_Hash_Tuple *t = data;
        char desc[100];
        sprintf(desc, "[%s] - Run %s on server",
                      (char *)t->key, (char *)t->data);

        gotham_modules_command_add("sysinfo", t->key, desc);
     }
   eina_iterator_free(it);
}


/**
 * @Brief Remove botman commands.
 * @param obj Module object.
 */
void
botman_commands_del(Module_Sysinfo *obj)
{
   Eina_Iterator *it;
   void *data;

   gotham_modules_command_del("sysinfo", ".sysinfo");

   /* Run through obj->commands and add matching commands */
   it = eina_hash_iterator_key_new(obj->commands);

   while(eina_iterator_next(it, &data))
     {
        const char *key = data;
        gotham_modules_command_del("sysinfo", key);
     }
   eina_iterator_free(it);
}


/**
 * @}
 */