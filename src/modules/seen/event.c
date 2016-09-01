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

void
event_command_seen(void *data,
                   Gotham_Citizen_Command *command)
{
   Module_Seen *seen = data;
   const char *s;

   EINA_SAFETY_ON_TRUE_RETURN(command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN);

   DBG("seen[%p] command[%p][%s]", seen, command, command->name);

   if ((seen->access_allowed) &&
       (!seen->access_allowed(gotham_modules_command_get(".seen"),
                              command->citizen)))
     {
        gotham_command_send(command, "Access denied");
        return;
     }

   if (!command->command[1])
     {
        gotham_command_send(command, "Usage : .seen pattern");
        return;
     }

   s = seen_query(seen, command->command[1]);
   gotham_command_send(command, s);
   free((char *)s);
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
