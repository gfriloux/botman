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
 * Define module name
 */
static const char *name = "Version";

/**
 * @brief Module init function.
 * Init needed libraries (eina), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   _module_version_log_dom = eina_log_domain_register("module_version",
                                                      EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Module register function.
 * Allocate module object, load conf and register functions.
 * @param gotham Gotham object
 * @return Module_Version object
 */
void *
module_register(Gotham *gotham)
{
   Module_Version *version;

   if ((gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN))
     return NULL;

   version = calloc(1, sizeof(Module_Version));
   if (!version)
     {
        ERR("Failed to alloc");
        return NULL;
     }
   version->gotham = gotham;

   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        version_botman_commands_register();
        version_botman_conf_load(version);
        return version;
     }
   version->vars = eina_array_new(1);
   version_alfred_commands_register();
   version_alfred_conf_load(version);
   return version;
}

/**
 * @brief Module unregister function.
 * Remove functions from list and free Module_Help structure
 * @param data Module_Version object
 */
void
module_unregister(void *data)
{
   Module_Version *version;
   Module_Version_Element *e;
   char *item;
   Eina_Array_Iterator iterator;
   unsigned int i;

   EINA_SAFETY_ON_NULL_RETURN(data);

   version = data;

   EINA_LIST_FREE(version->versions.list, e)
     {
        eina_stringshare_del(e->name);
        eina_stringshare_del(e->cmd);
     }
   eina_stringshare_del(version->versions.default_cmd);

   if (version->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        version_botman_commands_unregister();
        ecore_timer_del(version->poll);
        goto version_free;
     }

   EINA_ARRAY_ITER_NEXT(version->vars, i, item, iterator)
     free(item);
   eina_array_free(version->vars);
   version_alfred_commands_unregister();

version_free:
   free(version);
}

/**
 * @brief Module shutdown function.
 * Unregister from eina_log and shutdown eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_version_log_dom);
   eina_shutdown();
}

/**
 * @}
 */
