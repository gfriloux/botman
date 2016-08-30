#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Gotham.h>
#include <cJSON.h>
#include "gotham_private.h"

/**
 * @addtogroup Libgotham_Command_Functions
 * @{
 */

/**
 * Creates a new Gotham_Citizen_Command structure.
 * It represents an incoming message from a citizen.
 * Botman only talks in JSON format, so if we're an Alfred Citizen,
 * we first check if message is in JSON so we can parse it.
 * Otherwise message is readed as a classic one, (splitted as
 * in eina_str_split)
 * @param citizen Gotham_Citizen sending message
 * @param msg Message send
 * @return Newly created structure
 */
Gotham_Citizen_Command *
gotham_command_new(Gotham_Citizen *citizen,
                   const char *msg,
                   const char *jid)
{
   Gotham_Citizen_Command *command;
   cJSON *json_obj = NULL,
         *json_var;

   command = calloc(1, sizeof(Gotham_Citizen_Command));
   command->citizen = citizen;
   command->jid = strdup(jid);

   if ((citizen->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (msg[0] == '{')                                           &&
       (json_obj = cJSON_Parse(msg)))
     {
        json_var = json_obj->child;
        while (1)
          {
             if (!json_var) break;

             if (!strcmp(json_var->string, "command"))
               command->name = strdup(json_var->valuestring);
             else if (!strcmp(json_var->string, "parameters"))
               command->parameters = strdup(json_var->valuestring);
             else if (!strcmp(json_var->string, "status"))
               command->status = strdup(json_var->valuestring);
             else if (!strcmp(json_var->string, "content"))
               command->command = gotham_utils_json_array_stringify(json_var);

             json_var = json_var->next;
          }
        cJSON_Delete(json_obj);
     }
   else
     {
        const char **cmd = gotham_utils_msgtocommand(msg);
        if (!cmd)
          {
             ERR("Could not parse “%s”", msg);
             free(command);
             return NULL;
          }
        command->command = cmd;
        command->name = strdup(cmd[0]);
     }
   command->message = strdup(msg);
   return command;
}

Eina_Bool
gotham_command_send(Gotham_Citizen_Command *command,
                    const char *msg)
{
   if (!command->jid)
     return gotham_citizen_send(command->citizen, msg);

   return shotgun_message_send(command->citizen->gotham->shotgun,
                               command->jid,
                               msg,
                               SHOTGUN_MESSAGE_STATUS_ACTIVE,
                               (command->citizen->features.xhtml) ?
                                 EINA_TRUE :EINA_FALSE);
}

/**
 * Free a Gotham_Citizen_Command structure
 * @param command Command to free
 */
void
gotham_command_free(Gotham_Citizen_Command *command)
{
#define _FREE(a) if (command->a) free((char *)command->a);
   DBG("command[%p]", command);

   EINA_SAFETY_ON_NULL_RETURN(command);
   _FREE(name);
   _FREE(parameters);
   _FREE(status);
   _FREE(command[0]);
   _FREE(command);
   _FREE(message);
   free(command);
#undef _FREE
}

void
gotham_command_json_answer(const char *cmd,
                           const char *params,
                           Eina_Bool status,
                           Eina_Strbuf *content,
                           Gotham *gotham,
                           Gotham_Citizen *citizen,
                           Eina_Bool send_to_alfred)
{
   Gotham_Command_Json_Answer *gcja;
   char **split;
   char *s;
   unsigned int i;

   gcja = Gotham_Command_Json_Answer_new();

   gcja->command = eina_stringshare_add(cmd);
   gcja->parameters = eina_stringshare_add(params);
   gcja->status = eina_stringshare_add(status ? "ok" : "ko");

   split = eina_str_split(eina_strbuf_string_get(content), "\n", 0);
   for (i=0; split[i]; i++)
     if (split[i][0])
       gcja->content = eina_list_append(gcja->content, eina_stringshare_add(split[i]));

   s = gotham_serialize_struct_to_string(gcja, (Gotham_Serialization_Function)Gotham_Command_Json_Answer_to_azy_value);
   Gotham_Command_Json_Answer_free(gcja);
   gotham_citizen_send(citizen, s);

   if (send_to_alfred && (strcmp(citizen->jid, gotham->alfred->jid)))
     gotham_citizen_send(gotham->alfred, s);

   free(s);
}



/**
 * @}
 */

