#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>
#include "access.h"

/**
 * @addtogroup Gotham_Module_Access
 * @{
 */

/**
 * Defines module name
 */
static const char *name = "Access";

/**
 * @brief Access Module init function.
 * Init needed libraries (eina), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   _module_access_log_dom = eina_log_domain_register("module_access",
                                                     EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Access Module register function.
 * Allocate module object, add functions and load conf.
 * @param gotham Gotham object
 * @return Access module structure
 */
void *
module_register(Gotham *gotham)
{
   Module_Access *access;

   if ((gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN))
     {
        NFO("Cant load for this gotham type");
        return NULL;
     }

   access = calloc(1, sizeof(Module_Access));
   if (!access)
     {
        ERR("Failed to alloc");
        return NULL;
     }

   access->gotham = gotham;

   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     alfred_commands_register();

   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        botman_commands_register();
        botman_access_alfred_add(access);
     }

   access->conf = gotham_serialize_file_to_struct(MODULE_ACCESS_CONF, (Gotham_Deserialization_Function)azy_value_to_Module_Access_Conf);
   return access;
}

/**
 * @brief Access Module unregister function.
 * Remove functions from list, free all objects and then
 * free Module_Access structure
 * @param data Module_Access structure
 */
void
module_unregister(void *data)
{
   Module_Access *access = data;

   EINA_SAFETY_ON_NULL_RETURN(access);

   DBG("access[%p]", access);

   Module_Access_Conf_free(access->conf);

   if (access->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     alfred_commands_unregister();
   else if (access->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     botman_commands_unregister();

   free(access);
}

/**
 * @brief Access Module shutdown function.
 * Unregister from eina_log and shutdown eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_access_log_dom);
   eina_shutdown();
}

/**
 * @brief Check citizen authorization for using one of this module's command.
 * @param mc Gotham_Module_Command that citizen wants to use
 * @param citizen Gotham_Citizen being checked
 * @return EINA_TRUE if citizen is authorized, otherwise EINA_FALSE
 */
Eina_Bool
access_allowed(Gotham_Module_Command *mc,
               Gotham_Citizen *citizen)
{
   const char *tmp;
   int cmd_lvl,
       ctz_lvl;

   DBG("mc[%p] citizen[%p]", mc, citizen);

   EINA_SAFETY_ON_NULL_RETURN_VAL(mc, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(citizen, EINA_FALSE);


   tmp = gotham_modules_command_var_get(mc, "access_level");
   cmd_lvl = tmp ? atoi(tmp) : 1;

   tmp = gotham_citizen_var_get(citizen, "access_level");
   ctz_lvl = tmp ? atoi(tmp) : 1;

   DBG("Citizen=%d Command=%d", ctz_lvl, cmd_lvl);

   return (ctz_lvl >= cmd_lvl) ? EINA_TRUE : EINA_FALSE;
}

/**
 * @}
 */
