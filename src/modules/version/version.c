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
   Module_Version_Conf_Software *software;
   Eina_List *l;

   version = calloc(1, sizeof(Module_Version));
   EINA_SAFETY_ON_NULL_RETURN_VAL(version, NULL);

   version->gotham = gotham;
   version->conf = gotham_serialize_file_to_struct(MODULE_VERSION_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Version_Conf);
   EINA_SAFETY_ON_NULL_GOTO(version->conf, free_version);

   EINA_LIST_FOREACH(version->conf->softwares, l, software)
     {
        if ((!software->command) || (!strlen(software->command)))
          {
             eina_stringshare_del(software->command);
             software->command = eina_stringshare_printf(version->conf->default_cmd, software->name);
             DBG("Command for software [%s] : [%s]", software->name, software->command);
          }
     }

   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     version_botman_commands_register();
   else version_alfred_commands_register();
   return version;

free_version:
   free(version);
   return NULL;
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

   EINA_SAFETY_ON_NULL_RETURN(data);

   version = data;

   Module_Version_Conf_free(version->conf);

   if (version->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        version_botman_commands_unregister();
        ecore_timer_del(version->poll);
     }
   else version_alfred_commands_unregister();

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
