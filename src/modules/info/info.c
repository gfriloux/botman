#include "info.h"

static const char *name = "Info";

void *
module_register(Gotham *gotham)
{
   Module_Info *info;

   info = calloc(1, sizeof(Module_Info));
   EINA_SAFETY_ON_NULL_RETURN_VAL(info, NULL);

   DBG("info[%p]", info);

   info->gotham = gotham;

   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     info_conf_alfred_load(info);
   else
     info->sync = ecore_timer_add(20, info_botman_sync, info);

   return info;
}

void
module_unregister(void *data)
{
   Module_Info *info = data;
   char *s;
   unsigned int i;
   Eina_Array_Iterator iterator;

   DBG("info[%p]", info);

   EINA_ARRAY_ITER_NEXT(info->search_vars, i, s, iterator)
     free(s);
   eina_array_free(info->search_vars);

   if (info->sync) ecore_timer_del(info->sync);

   free(info);
}

void *
module_init(void)
{
   eina_init();

   _module_info_log_dom = eina_log_domain_register("module_info", EINA_COLOR_YELLOW);

   return (void *)name;
}

void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_info_log_dom);
   eina_shutdown();
}

void
module_json_answer(const char *cmd,
                   const char *params,
                   Eina_Bool status,
                   Eina_Strbuf *content,
                   Gotham *gotham,
                   Gotham_Citizen *citizen,
                   Eina_Bool send_to_alfred)
{
   cJSON *json_obj,
         *json_content;
   char **split;
   unsigned int i;
   char *ptr;

   json_obj = cJSON_CreateObject();
   cJSON_AddStringToObject(json_obj, "command", cmd);
   cJSON_AddStringToObject(json_obj, "parameters", params);
   cJSON_AddStringToObject(json_obj, "status", status ? "ok" : "ko");

   json_content = cJSON_CreateArray();
   split = eina_str_split(eina_strbuf_string_get(content), "\n", 0);
   for (i=0; split[i]; i++)
     if (split[i][0])
       cJSON_AddItemToArray(json_content, cJSON_CreateString(split[i]));

   cJSON_AddItemToObject(json_obj, "content", json_content);
   if (split[0]) free(split[0]);
   free(split);

   ptr = cJSON_Print(json_obj);
   cJSON_Delete(json_obj);
   gotham_citizen_send(citizen, ptr);

   if (send_to_alfred && (strcmp(citizen->jid, gotham->alfred->jid)))
     gotham_citizen_send(gotham->alfred, ptr);

   free(ptr);
   return;
}

