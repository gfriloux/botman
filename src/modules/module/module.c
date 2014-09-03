#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "module.h"

/**
 * @addtogroup Gotham_Module_Module
 * @{
 */

/**
 * Define module name
 */
static const char *name = "Module";

/**
 * @brief Module Module init function.
 * Init needed libraries (eina), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();

   _module_module_log_dom = eina_log_domain_register("module_module",
                                                     EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Module Module register function.
 * Allocate module object and register functions.
 * @param gotham Gotham object
 * @return Module_Module object
 */
void *
module_register(Gotham *gotham)
{
   Module_Module *mod;

   mod = calloc(1, sizeof(Module_Module));
   if (!mod)
     {
        ERR("Faild to alloc");
        return NULL;
     }
   mod->gotham = gotham;

   gotham_modules_command_add("module", ".module",
                              "[.module] - "
                              "This command allows you to "
                              "list loaded modules.");
   gotham_modules_command_add("module", ".module load",
                              "[.module load module_name] - "
                              "This command allows you to load a module.");
   gotham_modules_command_add("module", ".module unload",
                              "[.module unload module_name] - "
                              "This command allows you to unload a module.");
   gotham_modules_command_add("module", ".module reload",
                              "[.module reload module_name] - "
                              "This command allows you to reload a module.");
   mod->access_allowed = gotham_modules_function_get("access",
                                                     "access_allowed");
   return mod;
}

/**
 * @brief Module Module unregister function.
 * Remove functions from list and free Module_Help structure
 * @param data Module_Module object
 */
void
module_unregister(void *data)
{
   Module_Module *mod = data;

   EINA_SAFETY_ON_NULL_RETURN(mod);
   gotham_modules_command_del("module", ".module list");
   gotham_modules_command_del("module", ".module load");
   gotham_modules_command_del("module", ".module unload");
}

/**
 * @brief Module_Module shutdown function.
 * Unregister from eina_log and shutdown eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_module_log_dom);
   eina_shutdown();
}

/**
 * @}
 */
