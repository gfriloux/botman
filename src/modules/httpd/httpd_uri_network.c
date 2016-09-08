#include "httpd.h"

typedef struct _Httpd_Uri_Network
{
   Azy_Server_Module *module;
} Httpd_Uri_Network;

void
httpd_uri_network_init(void)
{
   httpd_module_add("/network", httpd_uri_network, NULL,
                    "Documentation for /network");
}

const char *
_httpd_uri_network_prepare(Eina_List *answers)
{
   cJSON *json;
   Eina_List *l;
   char *s;
   Module_Httpd_Queue_Message *mhqm;
   json = cJSON_CreateArray();
   EINA_LIST_FOREACH(answers, l, mhqm)
     {
        cJSON *answer;
        answer = cJSON_CreateObject();
        cJSON_AddItemToObject(answer, "jid", cJSON_CreateString(mhqm->jid));
        cJSON_AddItemToObject(answer, "message", cJSON_CreateString(mhqm->message ? mhqm->message : ""));
        cJSON_AddItemToArray(json, answer);
     }
   s = cJSON_Print(json);
   cJSON_Delete(json);
   return s;
}

void
httpd_uri_network_done(void *data,
                       Httpd_Queue_State state,
                       Eina_List *answers)
{
   Httpd_Uri_Network *hun = data;
   const char *s;

   s = _httpd_uri_network_prepare(answers);
   switch (state)
     {
        case QUEUE_NO_RESULTS:
          {
             httpd_uri_data(hun->module, NULL, 0, 204);
             break;
          }
        case QUEUE_INCOMPLETE:
          {
             httpd_uri_data(hun->module, (char *)s, strlen(s), 206);
             break;
          }
        case QUEUE_COMPLETE:
        default:
          httpd_uri_data(hun->module, (char *)s, strlen(s), 200);
     }
   azy_server_module_events_resume(hun->module, EINA_TRUE);
   free((char *)s);
}

void
httpd_uri_network(void *service_data EINA_UNUSED,
                  Azy_Server_Module *module,
                  const char *uri,
                  Azy_Net_Data *data EINA_UNUSED)
{
   char *p,
        **params;
   unsigned int nb;
   Eina_Strbuf *buf;
   Httpd_Uri_Network *hun;

   DBG("uri[%s]", uri);

   params = eina_str_split_full(uri, "/", 3, &nb);
   EINA_SAFETY_ON_NULL_GOTO(params, error);

   p = (params[2] && params[2][0]) ? params[2] : "*";
   EINA_SAFETY_ON_TRUE_GOTO(strlen(p) > 200, error_free_params);

   hun = calloc(1, sizeof(Httpd_Uri_Network));
   EINA_SAFETY_ON_NULL_GOTO(hun, error_free_params);

   hun->module = module;

   buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_GOTO(buf, error_free_hun);

   eina_strbuf_append_printf(buf, ".network %s", params[2]);

   httpd_spam_new(p, ".network", eina_strbuf_string_get(buf), httpd_uri_network_done, hun);

   azy_server_module_events_suspend(module);

   eina_strbuf_free(buf);
   free(params[0]);
   free(params);
   return;

error_free_hun:
   free(hun);
error_free_params:
   free(params[0]);
   free(params);
error:
   httpd_uri_data(module, NULL, 0, 500);
}
