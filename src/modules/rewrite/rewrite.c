#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>
#include "rewrite.h"

/**
 * @addtogroup Gotham_Module_Rewrite
 * @{
 */

/**
 * Define module name
 */
static const char *name = "Rewrite";

/**
 * @brief Callback for eina_has freeing data.
 * It's a fake, it does not free anything.
 * @param data UNUSED
 */
static void
_rewrite_rw_free_cb(void *data EINA_UNUSED)
{
   return;
}

/**
 * @brief Module Rewrite init function.
 * Init needed libraries (eina), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   _module_rewrite_log_dom = eina_log_domain_register("module_rewrite",
                                                      EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Module Rewrite register function.
 * Allocate module object, load conf.
 * For each rewrite rule found in conf, register a new function
 * @param gotham Gotham object
 * @return Module_Rewrite object
 */
void *
module_register(Gotham *gotham)
{
   Module_Rewrite *rewrite;
   Module_Rewrite_Conf_Rule *rule;
   Eina_List *l;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED)
     return NULL;

   rewrite = calloc(1, sizeof(Module_Rewrite));
   if (!rewrite)
     {
        ERR("Failed to alloc");
        return NULL;
     }

   rewrite->conf = gotham_serialize_file_to_struct(MODULE_REWRITE_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Rewrite_Conf);

   rewrite->rw = eina_hash_pointer_new(_rewrite_rw_free_cb);

   gotham_modules_command_add("rewrite", ".rewrite",
                              "[.rewrite] - "
                              "This command will list all know rewrite rules.",
                              event_citizen_command_list);

   EINA_LIST_FOREACH(rewrite->conf->rules, l, rule)
     gotham_modules_command_add("rewrite", rule->name, rule->description, NULL);

   return rewrite;
}

/**
 * @brief Module Rewrite unregister function.
 * Remove functions from list and free Module_Help structure
 * @param data Module_Rewrite object
 */
void
module_unregister(void *data)
{
   Module_Rewrite *rewrite = data;
   Module_Rewrite_Conf_Rule *rule = NULL;
   Eina_List *l,
             *l2;

   EINA_SAFETY_ON_NULL_RETURN(rewrite);
   DBG("rewrite[%p]", rewrite);

   gotham_modules_command_del("rewrite", ".rewrite");

   EINA_LIST_FOREACH_SAFE(rewrite->conf->rules, l, l2, rule)
     {
        gotham_modules_command_del("rewrite", rule->name);
        rewrite->conf->rules = eina_list_remove(rewrite->conf->rules, rule);
        Module_Rewrite_Conf_Rule_free(rule);
     }
   eina_hash_free(rewrite->rw);
   free(rewrite);
}

/**
 * @brief Module_Rewrite shutdown function.
 * Unregister from eina_log and shutdown eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_rewrite_log_dom);
   eina_shutdown();
}

/**
 * @}
 */
