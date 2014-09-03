#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <fnmatch.h>

#include "seen.h"

/**
 * @addtogroup Gotham_Module_Seen
 * @{
 */

/**
 * Define module name
 */
static const char *name = "Seen";

/**
 * @brief Module Seen init function.
 * Init needed libraries (eina), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   _module_seen_log_dom = eina_log_domain_register("module_seen",
                                                   EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Module Seen register function.
 * Allocate module object, load conf and register functions.
 * @param gotham Gotham object
 * @return Module_Seen object
 */
void *
module_register(Gotham *gotham)
{
   Module_Seen *seen;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED)
     {
        NFO("Cant load for this gotham type");
        return NULL;
     }

   seen = calloc(1, sizeof(Module_Seen));
   if (!seen)
     {
        ERR("Faild to alloc");
        return NULL;
     }
   seen->gotham = gotham;
   seen->vars = eina_array_new(1);

   seen_conf_load(seen);


   gotham_modules_command_add("seen", ".seen",
                              "[.seen pattern] - "
                              "This command allows you to search for XMPP "
                              "users. It uses JID to search them, but "
                              "additional criterias can be specified in "
                              "seen.conf.");

   seen->access_allowed = gotham_modules_function_get("access",
                                                      "access_allowed");
   return seen;
}

/**
 * @brief Module Seen unregister function.
 * Remove functions from list and free Module_Help structure
 * @param data Module_Seen object
 */
void
module_unregister(void *data)
{
   Module_Seen *seen = data;
   char *item;
   Eina_Array_Iterator iterator;
   unsigned int i;

   EINA_SAFETY_ON_NULL_RETURN(seen);
   EINA_ARRAY_ITER_NEXT(seen->vars, i, item, iterator)
     free(item);
   gotham_modules_command_del("seen", ".seen");
   eina_array_free(seen->vars);
}

/**
 * @brief Module_Seen shutdown function.
 * Unregister from eina_log and shutdown eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_seen_log_dom);
   eina_shutdown();
}

/**
 * @}
 */
