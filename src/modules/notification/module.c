#include "main.h"

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
   char *ptr;
   const char *s;

   json_obj = cJSON_CreateObject();
   cJSON_AddStringToObject(json_obj, "command", cmd);
   cJSON_AddStringToObject(json_obj, "parameters", params);
   cJSON_AddStringToObject(json_obj, "status", status ? "ok" : "ko");

   json_content = cJSON_CreateArray();

   if (content)
     {
        s = eina_strbuf_string_get(content);
        if (s) cJSON_AddItemToArray(json_content, cJSON_CreateString(s));
     }

   cJSON_AddItemToObject(json_obj, "content", json_content);

   ptr = cJSON_Print(json_obj);
   cJSON_Delete(json_obj);
   gotham_citizen_send(citizen, ptr);

   if (send_to_alfred && (strcmp(citizen->jid, gotham->alfred->jid)))
     gotham_citizen_send(gotham->alfred, ptr);

   free(ptr);
   return;
}
