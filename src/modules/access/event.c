#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include "access.h"

/**
 * @addtogroup Gotham_Module_Access
 * @{
 */


/**
 * Authorization macro
 */
#define AUTH(_a, _b)                                                           \
   {                                                                           \
      if (!access_allowed(gotham_modules_command_get(_a), _b))                 \
        {                                                                      \
           gotham_citizen_send(_b, "Access denied");                           \
           return EINA_TRUE;                                                   \
        }                                                                      \
   }

/**
 * @brief Callback when all modules are loaded.
 * Run through modules list and set access level for each, using
 * @ref modules_commands_check
 * @param data Module_Access structure
 * @param type UNUSED
 * @param ev Gotham structure
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev)
{
   Module_Access *access = data;
   Gotham *gotham = ev;
   Gotham_Module *module;

   DBG("access[%p] gotham[%p]", access, gotham);
   EINA_INLIST_FOREACH(gotham_modules_list(), module)
     modules_commands_check(access, module);
   return EINA_TRUE;
}


/**
 * @brief Callback when a single module is loaded.
 * Call @ref modules_commands_check to check / set access level to command
 * @param data Module_Access structure
 * @param type UNUSED
 * @param ev Gotham_Module structure
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_single_ready(void *data,
                           int type EINA_UNUSED,
                           void *ev)
{
   Module_Access *access = data;
   Gotham_Module *module = ev;

   DBG("access[%p] module[%p]", access, module);

   modules_commands_check(access, module);

   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen connects.
 * call @ref citizen_access_eval to check / set access level to citizen
 * @param data Module_Access structure
 * @param type UNUSED
 * @param ev Gotham_Citizen structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_connect(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Access *access = data;
   Gotham_Citizen *citizen = ev;

   citizen_access_eval(access, citizen);

   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * @param data Module_Access structure
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Access *access = data;
   Gotham_Citizen_Command *command = ev;
   const char **access_command = command->command;
   const char *answer = NULL;

   DBG("access[%p] command[%p]=%s", access, command, command->name);

   if (strcmp(command->name, ".access"))
     return EINA_TRUE;

   if ((access->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
       (command->citizen->type == GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (!strcmp(access_command[1], "set")))
     {
        botman_access_sync(access, command);
        command->handled = EINA_TRUE;
        return EINA_TRUE;
     }

   if (!access_command[1])
     {
        command->handled = EINA_TRUE;
        AUTH(".access", command->citizen);
        answer = citizen_access_list(access);
     }
   else if (access->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;
   else if(!strcmp(access_command[1], "set"))
     {
        command->handled = EINA_TRUE;
        AUTH(".access set", command->citizen);
        answer = citizen_access_set(access, command);
     }
   else if(!strcmp(access_command[1], "del"))
     {
        command->handled = EINA_TRUE;
        AUTH(".access del", command->citizen);
        answer = citizen_access_del(access, command);
     }
   else if(!strcmp(access_command[1], "add"))
     {
        command->handled = EINA_TRUE;
        AUTH(".access add", command->citizen);
        answer = citizen_access_add(access, command);
     }
   else if(!strcmp(access_command[1], "sync"))
     {
        command->handled = EINA_TRUE;
        AUTH(".access sync", command->citizen);
        answer = citizen_access_sync(access);
     }

   gotham_citizen_send(command->citizen,
                       (answer) ? answer : "Failed to execute command");
   free((char *)answer);
   return EINA_TRUE;
}

#undef AUTH

/**
 * @}
 */
