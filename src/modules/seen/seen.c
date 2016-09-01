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

   seen->conf = gotham_serialize_file_to_struct(MODULE_SEEN_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Seen_Conf);

   {
      Eina_List *l;
      const char *s;
      EINA_LIST_FOREACH(seen->conf->vars, l, s)
        DBG("Found custom variable [%s]", s);
   }

   gotham_modules_command_add("seen", ".seen",
                              "[.seen pattern] - "
                              "This command allows you to search for XMPP "
                              "users. It uses JID to search them, but "
                              "additional criterias can be specified in "
                              "seen.conf.", event_command_seen);

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

   EINA_SAFETY_ON_NULL_RETURN(seen);
   Module_Seen_Conf_free(seen->conf);
   gotham_modules_command_del("seen", ".seen");
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
