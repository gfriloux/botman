#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "version.h"

/**
 * @addtogroup Gotham_Module_Version
 * @{
 */

/**
 * Authorization macro
 */
#define AUTH(_a, _b, _c)                                                       \
   {                                                                           \
      if ((_a) && (!_a(_b, _c)))                                               \
        {                                                                      \
           gotham_citizen_send(_c, "Access denied");                           \
           return EINA_TRUE;                                                   \
        }                                                                      \
   }
/* "debug" */

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Version *version = data;
   version->access_allowed = gotham_modules_function_get("access",
                                                         "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * @param data Module object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Version *version = data;
   Gotham_Citizen_Command *command = ev;

   EINA_SAFETY_ON_NULL_RETURN_VAL(version, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   if (strcmp(command->name, ".version"))
     return EINA_TRUE;

   DBG("version[%p] command[%p]=%s", version, command, command->name);

   command->handled = EINA_TRUE;

   /*
    * Check autorization for .version find command
    * (only valid if we're talking to alfred)
    */

   if (version->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        AUTH(version->access_allowed, gotham_modules_command_get(".version"),
             command->citizen);
        version_botman_command(version, command);
        return EINA_TRUE;
     }

   /* We are alfred. */

   /* We receive a .version find command */
   if ((command->command[1]) && (!strcmp(command->command[1], "find")))
     {
        AUTH(version->access_allowed,
             gotham_modules_command_get(".version find"),
             command->citizen);
        version_alfred_find_command(version, command);
        return EINA_TRUE;
     }

   /* A botman sends us his .version */
   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        version_alfred_answer_get(version, command);
        return EINA_TRUE;
     }

   /* We're talking to alfred, he has to search in his cached info */
   AUTH(version->access_allowed, gotham_modules_command_get(".version"),
        command->citizen);
   version_alfred_command(version, command);

   return EINA_TRUE;
}

/**
 * @brief Callback for a citizen connection.
 * @param data Module object
 * @param type UNUSED
 * @param ev Gotham_Citizen that has disconnected
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_connect(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Version *version = data;
   Gotham_Citizen *citizen = ev;
   char *s;
   Eina_Bool updated;

   EINA_SAFETY_ON_NULL_RETURN_VAL(version, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(citizen, EINA_TRUE);

   DBG("version[%p] citizen[%p]=“%s” type %d",
       version, citizen, citizen->jid, citizen->type);

   if (version->gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if ((!version->gotham->alfred) ||
       (citizen->type != GOTHAM_CITIZEN_TYPE_ALFRED) ||
       (version->sent_once))
     return EINA_TRUE;

   s = version_botman_fetch(version, &updated);
   if (!s)
     {
        ERR("Failed to fetch version infos");
        return EINA_TRUE;
     }
   DBG("Sending version info to alfred");
   gotham_citizen_send(version->gotham->alfred, s);
   version->sent_once = EINA_TRUE;
   free(s);
   return EINA_TRUE;
}

/**
 * @brief Callback when we are connected.
 * If DB info are already loaded, set and send vCard using
 * @ref botman_vcard_send
 * @param data Module_Asp64 structure
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_connect(void *data,
              int type EINA_UNUSED,
              void *ev EINA_UNUSED)
{
   Module_Version *version = data;
   char *s;
   Eina_Bool updated;

   DBG("version[%p]", version);

   if (version->gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   version->poll = ecore_timer_add(60.0, version_botman_poll, version);

   if ((!version->gotham->alfred) ||
       (version->gotham->alfred->status == GOTHAM_CITIZEN_STATUS_OFFLINE))
     return EINA_TRUE;

   s = version_botman_fetch(version, &updated);
   if (!s)
     {
        ERR("Failed to fetch version infos");
        return EINA_TRUE;
     }

   DBG("Sending version info to alfred");
   gotham_citizen_send(version->gotham->alfred, s);
   version->sent_once = EINA_TRUE;
   free(s);
   return EINA_TRUE;
}

#undef AUTH

/**
 * @}
 */

