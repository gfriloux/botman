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
gotham_command_new(Gotham_Citizen *citizen, const char *msg)
{
   Gotham_Citizen_Command *command;
   cJSON *json_obj = NULL,
         *json_var;

   command = calloc(1, sizeof(Gotham_Citizen_Command));
   command->citizen = citizen;
   if ((citizen->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (msg[0] == '{')                                               &&
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

/**
 * @}
 */

