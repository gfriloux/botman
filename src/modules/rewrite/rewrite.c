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
   Module_Rewrite_Rule *rule;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED)
     return NULL;

   rewrite = calloc(1, sizeof(Module_Rewrite));
   if (!rewrite)
     {
        ERR("Failed to alloc");
        return NULL;
     }

   conf_load(rewrite);
   rewrite->rw = eina_hash_pointer_new(_rewrite_rw_free_cb);

   gotham_modules_command_add("rewrite", ".rewrite",
                              "[.rewrite] - "
                              "This command will list all know rewrite rules.");

   EINA_INLIST_FOREACH(rewrite->rules, rule)
     {
        gotham_modules_command_add("rewrite", rule->name, rule->desc);
     }

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
   Module_Rewrite_Rule *rule;
   Eina_Inlist *l;

   EINA_SAFETY_ON_NULL_RETURN(rewrite);
   DBG("rewrite[%p]", rewrite);

   gotham_modules_command_del("rewrite", ".rewrite");

   EINA_INLIST_FOREACH_SAFE(rewrite->rules, l, rule)
     {
        gotham_modules_command_del("rewrite", rule->name);
        rewrite->rules = eina_inlist_remove(rewrite->rules,
                                            EINA_INLIST_GET(rule));
        free((char *)rule->name);
        free((char *)rule->filter);
        eina_stringshare_del(rule->rule);
        free((char *)rule->desc);
        free(rule);
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
