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
 * @}
 */
