#include "info.h"

void
info_conf_alfred_load(Module_Info *info)
{
   cJSON *json,
         *json_sv,
         *p;

   json = gotham_modules_conf_load(MODULE_INFO_CONF);
   EINA_SAFETY_ON_NULL_RETURN(json);

   json_sv = cJSON_GetObjectItem(json, "search_vars");
   EINA_SAFETY_ON_NULL_GOTO(json_sv, free_json);

   info->search_vars = eina_array_new(1);
   EINA_SAFETY_ON_NULL_GOTO(info->search_vars, free_json);

   p = json_sv->child;
   while (p)
     {
        char *s;

        s = strdup(p->valuestring);
        eina_array_push(info->search_vars, s);
        p = p->next;
     }

free_json:
   cJSON_Delete(json);
}
