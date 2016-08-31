#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cJSON.h>
#include "sysinfo.h"

/**
 * @addtogroup Gotham_Module_Sysinfo
 * @{
 */
void
_conf_commands_load(cJSON *json, Eina_Hash *h)
{
   int vars_count;

   vars_count = cJSON_GetArraySize(json);
   if (!vars_count)
     return;

   json = cJSON_GetArrayItem(json, 0);
   if (!json)
     return;

   json = json->child;

   while (1)
     {
        if (!json) break;
        NFO("Found var %s : %s", json->string, json->valuestring);
        eina_hash_direct_add(h,
                             strdup(json->string),
                             strdup(json->valuestring));

        json = json->next;
     }
}

/**
 * @brief Load conf file for tunnel vars.
 * @param obj Module object
 */
void
conf_load(Module_Sysinfo *obj)
{
   cJSON *json,
         *json_vars,
         *var;
   int i,
       vars_count;

   json = gotham_modules_conf_load(MODULE_CONF);
   if (!json)
     goto func_end;

   /* Get hardware commands */
   _conf_commands_load(cJSON_GetObjectItem(json, "hw"), obj->hw);
   _conf_commands_load(cJSON_GetObjectItem(json, "commands"), obj->commands);

   /* Get vars */
   json_vars = cJSON_GetObjectItem(json, "vars");
   if (!json_vars)
     goto func_end;

   vars_count = cJSON_GetArraySize(json_vars);

   for (i=0; i < vars_count; i++)
     {
        var = cJSON_GetArrayItem(json_vars, i);
        NFO("Found var %s", var->valuestring);
        obj->vars = eina_list_append(obj->vars, strdup(var->valuestring));
     }

func_end:
   cJSON_Delete(json);
}

/**
 * @}
 */
