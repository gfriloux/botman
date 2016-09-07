#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#include "sysinfo.h"

/**
 * @addtogroup Gotham_Module_Sysinfo
 * @{
 */

/**
 * Define module name
 */
static const char *name = "Sysinfo";

/**
 * @brief Module init function.
 * Init needed libraries (eina, ecore), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   ecore_init();
   _module_log_dom = eina_log_domain_register("module_sysinfo",
                                              EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Module register function.
 * Allocate module object and register functions.
 * @param gotham Gotham object
 * @return Module object
 */
void *
module_register(Gotham *gotham)
{
   Module_Sysinfo *obj;

   obj = calloc(1, sizeof(Module_Sysinfo));
   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, NULL);

   obj->gotham = gotham;
   obj->conf = gotham_serialize_file_to_struct(MODULE_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Sysinfo_Conf);

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        /* Gather sysinfo and store as custom vars */
        botman_sysinfo_get(obj);
        botman_commands_add(obj);
     }
   else
     alfred_commands_add(obj);

   return obj;
}

/**
 * @brief Module unregister function.
 * Remove functions from list and free Module object
 * @param data Module object
 */
void
module_unregister(void *data)
{
   Module_Sysinfo *obj = data;

   EINA_SAFETY_ON_NULL_RETURN(obj);

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     botman_commands_del(obj);

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     alfred_commands_del(obj);

   Array_Module_Sysinfo_Conf_Item_free(obj->conf->hw);
   Array_Module_Sysinfo_Conf_Item_free(obj->conf->commands);
   Array_string_free(obj->conf->vars);

   free(obj);
}

/**
 * @brief Module shutdown function.
 * Unregister from eina_log and shutdown eina and ecore
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_log_dom);
   ecore_shutdown();
   eina_shutdown();
}

/**
 * @}
 */
