#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "install.h"

/**
 * @addtogroup Gotham_Module_Install
 * @{
 */

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module_Install object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Install *install = data;
   install->access_allowed = gotham_modules_function_get("access",
                                                         "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * If ok, launch command.
 * @param data Module_Install object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Install *install = data;
   Gotham_Citizen_Command *command = ev;
   Module_Install_Type mod_type = MODULE_INSTALL_TYPE_WAZUFUK;
   Eina_Bool auth = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(install, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   if (install->gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if (!strcmp(command->name, ".install"))
     mod_type = MODULE_INSTALL_TYPE_INSTALL;
   else if (!strcmp(command->name, ".upgrade"))
     mod_type = MODULE_INSTALL_TYPE_UPGRADE;
   else if (!strcmp(command->name, ".jobs"))
     mod_type = MODULE_INSTALL_TYPE_JOBS;
   else if (!strcmp(command->name, ".kill"))
     mod_type = MODULE_INSTALL_TYPE_KILL;
   else
     return EINA_TRUE;

   DBG("install[%p] command[%p]=%s", install, command, command->name);

   command->handled = EINA_TRUE;

   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     auth = EINA_TRUE;
   else if ((install->access_allowed) &&
            (!install->access_allowed(gotham_modules_command_get(command->name),
                                      command->citizen)))
     auth = EINA_FALSE;
   else
     auth = EINA_TRUE;

   if (!auth)
     {
        ERR("Access denied for %s", command->citizen->jid);
        gotham_command_send(command, "Access denied");
        return EINA_TRUE;
     }

   // Botman receives the message
   module_install_event_botman_command(install, mod_type, command);

   return EINA_TRUE;
}

/**
 * @}
 */
