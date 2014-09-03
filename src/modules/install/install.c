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
 * Define module name
 */
static const char *name = "Install";

/**
 * @brief Callback for freeing a eina_hash value from an install object
 * @param data data to free
 */
static void
_module_install_hash_free_cb(void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(data);
   free(data);
}

/**
 * @brief Callback for freeing a eina_hash from a job object
 * @param data data to free
 */
static void
_job_free_cb(void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(data);
   free(data);
}

/**
 * @brief Install Module init function.
 * Init needed libraries (eina), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   _module_install_log_dom = eina_log_domain_register("module_install",
                                                      EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Install Module register function.
 * Allocate module objects and register functions.
 * @param gotham Gotham object
 * @return Install module structure
 */
void *
module_register(Gotham *gotham)
{
   Module_Install *install;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return NULL;

   install = calloc(1, sizeof(Module_Install));
   if (!install)
     {
        ERR("Failed to alloc");
        return NULL;
     }

   install->gotham = gotham;

   install->install = eina_hash_string_small_new(_module_install_hash_free_cb);
   install->upgrade = eina_hash_string_small_new(_module_install_hash_free_cb);
   install->jobs = eina_hash_pointer_new(_job_free_cb);

   module_install_botman_conf_load(install);
   module_install_botman_commands_register();

   return install;
}

/**
 * @brief Install Module unregister function.
 * Remove functions from list and free Module_Help structure
 * @param data Module_Install structure
 */
void
module_unregister(void *data)
{
   Module_Install *install = data;

   EINA_SAFETY_ON_NULL_RETURN(install);

   module_install_botman_commands_unregister();
   eina_hash_free(install->install);
   eina_hash_free(install->upgrade);
   eina_hash_free(install->jobs);

   free(install);
}

/**
 * @brief Install Module shutdown function.
 * Unregister from eina_log and shutdown eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_install_log_dom);
   eina_shutdown();
}

/**
 * @}
 */
