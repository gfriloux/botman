#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <fnmatch.h>

#include "seen.h"

/**
 * @addtogroup Gotham_Module_Seen
 * @{
 */

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module_Seen object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Seen *seen = data;
   seen->access_allowed = gotham_modules_function_get("access",
                                                      "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * Then search citizens that match pattern and send back citizen list.
 * @param data Module_Module object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Seen *seen = data;
   Gotham_Citizen_Command *command = ev;
   const char **seen_cmd = command->command,
              *s;

   EINA_SAFETY_ON_NULL_RETURN_VAL(seen, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   DBG("seen[%p] command[%p]=%s", seen, command, command->name);

   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if (strcmp(command->name, ".seen"))
     return EINA_TRUE;

   command->handled = EINA_TRUE;

   if ((seen->access_allowed) &&
       (!seen->access_allowed(gotham_modules_command_get(".seen"),
                              command->citizen)))
     {
        gotham_command_send(command, "Access denied");
        return EINA_TRUE;
     }

   if (!seen_cmd[1])
     {
        gotham_command_send(command, "Usage : .seen pattern");
        return EINA_TRUE;
     }

   s = seen_query(seen, seen_cmd[1]);

   gotham_command_send(command, s);

   free((char *)s);
   return EINA_TRUE;
}

/**
 * @brief Callback for a citizen disconnection.
 * Update the seen_last custom var with disconnect time.
 * @param data Module_Seen object
 * @param type UNUSED
 * @param ev Gotham_Citizen that has disconnected
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_disconnect(void *data,
                         int type EINA_UNUSED,
                         void *ev)
{
   Module_Seen *seen = data;
   Gotham_Citizen *citizen = ev;

   DBG("seen[%p] citizen[%p] time=%.0f",
       seen, citizen, (double)time(0));


   VARSET("seen_last", "%.0f", (double)time(0));

   return EINA_TRUE;
}

/**
 * @}
 */
