#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cJSON.h>
#include "spam.h"

/**
 * @addtogroup Gotham_Module_Spam
 * @{
 */

/**
 * @brief Load conf for Module Spam.
 * @param spam Module_Spam object
 */
void
conf_load(Module_Spam *spam)
{
   cJSON *json,
         *json_vars;
   int i,
       vars_count;

   json = gotham_modules_conf_load(MODULE_SPAM_CONF);
   if (!json)
     return;

   json_vars = cJSON_GetObjectItem(json, "vars");
   vars_count = cJSON_GetArraySize(json_vars);

   for (i=0; i < vars_count; i++)
     {
        cJSON *var_name = cJSON_GetArrayItem(json_vars, i);
        NFO("Found var %s", var_name->valuestring);
        eina_array_push(spam->vars, strdup(var_name->valuestring));
     }

   cJSON_Delete(json);
}

/**
 * @}
 */
