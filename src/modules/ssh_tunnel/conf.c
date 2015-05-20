#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cJSON.h>
#include "ssh_tunnel.h"

/**
 * @addtogroup Gotham_Module_Ssh_Tunnel
 * @{
 */

/**
 * @brief Load conf file for tunnel vars.
 * @param obj Module object
 */
void
conf_load(Module_Ssh_Tunnel *obj)
{
   cJSON *json,
         *json_vars,
         *json_p;
   int i,
       vars_count;

   json = gotham_modules_conf_load(MODULE_CONF);
   if (!json)
     return;

   json_p = cJSON_GetObjectItem(json, "host");
   if (json_p) obj->infos.host = strdup(json_p->valuestring);

   json_p = cJSON_GetObjectItem(json, "login");
   if (json_p) obj->infos.login = strdup(json_p->valuestring);

   json_p = cJSON_GetObjectItem(json, "key");
   if (json_p) obj->infos.key = strdup(json_p->valuestring);

   json_p = cJSON_GetObjectItem(json, "port");
   if (json_p) obj->infos.port = json_p->valueint;
   else obj->infos.port = 22;

   json_vars = cJSON_GetObjectItem(json, "vars");
   if (!json_vars)
     return;

   vars_count = cJSON_GetArraySize(json_vars);

   for (i=0; i < vars_count; i++)
     {
        cJSON *var_name = cJSON_GetArrayItem(json_vars, i);
        NFO("Found var %s", var_name->valuestring);
        eina_array_push(obj->vars, strdup(var_name->valuestring));
     }

   cJSON_Delete(json);
}

/**
 * @}
 */
