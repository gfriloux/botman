#include "gdb.h"

static const char *name = "GDB";
int _gdb_log_dom = -1;

const char *
module_init(void)
{
   eina_init();
   _gdb_log_dom = eina_log_domain_register("module_gdb", EINA_COLOR_RED);
   ecore_init();

   DBG("void");

   return name;
}

void
module_shutdown(void)
{
   DBG("void");

   ecore_shutdown();
   eina_log_domain_unregister(_gdb_log_dom);
   eina_shutdown();
}

void *
module_register(Gotham *gotham)
{
   Module_Gdb *gdb;

   EINA_SAFETY_ON_TRUE_RETURN_VAL(gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN, NULL);

   gdb = calloc(1, sizeof(Module_Gdb));
   EINA_SAFETY_ON_NULL_RETURN_VAL(gdb, NULL);

   gdb->gotham = gotham;

   if (gdb->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) botman_register(gdb);
   else alfred_register(gdb);

   gdb->access_allowed = gotham_modules_function_get("access",
                                                     "access_allowed");
   return gdb;
}

void
module_unregister(void *data)
{
   Module_Gdb *gdb = data;
   Eina_List *l;
   char *s;

   EINA_LIST_FOREACH(gdb->dumps.known, l, s)
     {
        free(s);
     }

   ecore_timer_del(gdb->dumps.poll);
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
