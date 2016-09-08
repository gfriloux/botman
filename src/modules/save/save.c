#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include "save.h"

/**
 * @addtogroup Gotham_Module_Save
 * @{
 */

/**
 * Declare module name
 */
static const char *name = "Save";

/**
 * Declare module object
 */
Module_Save *_obj;

/**
 * @brief Module Save init function.
 * Init needed libraries (eina, ecore), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   ecore_init();
   _module_save_log_dom = eina_log_domain_register("module_save",
                                                   EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Module Save register function.
 * Allocate module object and register functions.
 * Also set backup timer
 * @param gotham Gotham object
 * @return Module_Save object
 */
void *
module_register(Gotham *gotham)
{
   _obj = calloc(1, sizeof(Module_Save));
   EINA_SAFETY_ON_NULL_RETURN_VAL(_obj, NULL);

   _obj->gotham = gotham;
   _obj->conf = gotham_serialize_file_to_struct(MODULE_SAVE_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Save_Conf);
   if (!_obj->conf)
     {
        _obj->conf = Module_Save_Conf_new();
        _obj->conf->interval = 60;
     }

   _obj->et_backup = ecore_timer_add(_obj->conf->interval,
                                     conf_backup, _obj);

   gotham_modules_command_add("save", ".save",
                              "[.save] - "
                              "This command will show informations "
                              "about the backup.",
                              event_citizen_save_info);
   gotham_modules_command_add("save", ".save set",
                              "[.save set variable value] - "
                              "This command allows you to set a variable. "
                              "Possible variables : 'interval'.",
                              event_citizen_save_set);
   return _obj;
}

/**
 * @brief Module Save unregister function.
 * Remove functions from list, remove timer
 * and free Module_Help structure
 * @param data Module_Save object
 */
void
module_unregister(void *data)
{
   Module_Save *save = data;
   ecore_timer_del(save->et_backup);
   free(save);
}

/**
 * @brief Module_Save shutdown function.
 * Unregister from eina_log and shutdown ecore and eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_save_log_dom);
   ecore_shutdown();
   eina_shutdown();
}

/**
 * @brief Function that can be called from outside this module.
 * Make a conf backup immediately
 */
void
save_conf(void)
{
   DBG("Making Backup now");
   conf_backup(_obj);
}

/**
 * @}
 */
