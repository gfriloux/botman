#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cJSON.h>
#include "install.h"

/**
 * @addtogroup Gotham_Module_Install
 * @{
 */

/**
 * @brief Load a JSON object into an Eina_Has obj.
 * @param h Eina_Hash to fill
 * @param json cJSON object to read
 */
static void
_module_install_element_vars_load(Eina_Hash *h, cJSON *json)
{
   cJSON *var;

   EINA_SAFETY_ON_NULL_RETURN(h);
   EINA_SAFETY_ON_NULL_RETURN(json);

   DBG("h[%p] json[%p]", h, json);

   var = json->child;

   while (1)
     {
        if (!var)
          break;

        if (var->type==cJSON_String)
          {
             DBG("Adding %s=%s to h[%p]", var->string, var->valuestring, h);
             eina_hash_direct_add(h,
                                  strdup(var->string),
                                  strdup(var->valuestring));
          }

        var = var->next;
     }
}

/**
 * @brief Load conf file (Alfred part only).
 * Load and parse conf file in a cJOSN object.
 * Then retrieve only vars (used for pattern searches)
 * @param install Module_Install object
 */
void
module_install_alfred_conf_load(Module_Install *install)
{
   cJSON *json,
          *json_obj;
   int i,
       vars_count;

   json = gotham_modules_conf_load(MODULE_INSTALL_CONF);
   if (!json)
     return;

   json_obj = cJSON_GetObjectItem(json, "vars");
   if (!json_obj)
     return;

   vars_count = cJSON_GetArraySize(json_obj);
   if (!vars_count)
     return;

   for (i=0; i < vars_count; i++)
     {
        cJSON *var_name = cJSON_GetArrayItem(json_obj, i);
        NFO("Found var %s", var_name->valuestring);
        eina_array_push(install->vars, strdup(var_name->valuestring));
     }

   cJSON_Delete(json);
}

/**
 * @brief Load conf file (Botman part only).
 * Load and parse conf file in a cJSON object.
 * Will search in conf file the settings for install and upgrade commands.
 * @param install Module_Install object
 */
void
module_install_botman_conf_load(Module_Install *install)
{
   cJSON *json,
         *json_install,
         *json_upgrade;

   EINA_SAFETY_ON_NULL_RETURN(install);

   DBG("install[%p]", install);

   json = gotham_modules_conf_load(MODULE_INSTALL_CONF);
   if (!json)
     return;

   DBG("Searching json object “install”");
   json_install = cJSON_GetObjectItem(json, "install");
   if ((json_install = cJSON_GetArrayItem(json_install, 0)))
     _module_install_element_vars_load(install->install, json_install);

   DBG("Searching json object “upgrade”");
   json_upgrade = cJSON_GetObjectItem(json, "upgrade");
   if ((json_upgrade = cJSON_GetArrayItem(json_upgrade, 0)))
     _module_install_element_vars_load(install->upgrade, json_upgrade);

   cJSON_Delete(json);
}

/**
 * @}
 */
