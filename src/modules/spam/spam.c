#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>
#include "spam.h"

/**
 * @addtogroup Gotham_Module_Spam
 * @{
 */

/**
 * Define module name
 */
static const char *name = "Spam";

/**
 * @brief Module Spam init function.
 * Init needed libraries (eina), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   log_dom = eina_log_domain_register("module_spam", EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Module Spam register function.
 * Allocate module object, load conf and register functions.
 * Also declare a timer for queue management.
 * @param gotham Gotham object
 * @return Module_Spam object
 */
void *
module_register(Gotham *gotham)
{
   Module_Spam *spam;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED)
     return NULL;

   spam = calloc(1, sizeof(Module_Spam));
   if (!spam)
     {
        ERR("Failed to alloc");
        return NULL;
     }

   spam->gotham = gotham;
   spam->conf = gotham_serialize_file_to_struct(MODULE_SPAM_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Spam_Conf);
   spam->access_allowed = gotham_modules_function_get("access",
                                                      "access_allowed");
   gotham_modules_command_add("spam", ".spam",
                              "[.spam pattern command] - "
                              "This command allows you to send commands "
                              "to multiple botmans at once.");
   spam->queue.t = ecore_timer_add(30, queue_timer, spam);
   return spam;
}

/**
 * @brief Module Spam unregister function.
 * Remove functions from list, remove timer and free Module_Help structure
 * @param data Module_Spam object
 */
void
module_unregister(void *data)
{
   Module_Spam *spam = data;

   EINA_SAFETY_ON_NULL_RETURN(spam);

   ecore_timer_del(spam->queue.t);
   Module_Spam_Conf_free(spam->conf);

   gotham_modules_command_del("spam", ".spam");
}

/**
 * @brief Module_Spam shutdown function.
 * Unregister from eina_log and shutdown eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(log_dom);
   eina_shutdown();
}

/**
 * @}
 */
