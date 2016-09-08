#include "httpd.h"

typedef struct _Httpd_Uri_Uptime
{
   Azy_Server_Module *module;
} Httpd_Uri_Uptime;

const char *
_httpd_uri_uptime_prepare(Eina_List *answers)
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

        DBG("Uptime from [%s] : [%s]", mhqm->jid, mhqm->message);

        cJSON_AddItemToArray(json, answer);
     }

   s = cJSON_Print(json);
   cJSON_Delete(json);

   return s;
}

void
httpd_uri_uptime_done(void *data,
                      Httpd_Queue_State state,
                      Eina_List *answers)
{
   Httpd_Uri_Uptime *huu = data;
   const char *s;

   s = _httpd_uri_uptime_prepare(answers);

   switch (state)
     {
        case QUEUE_NO_RESULTS:
          {
             httpd_uri_data(huu->module, NULL, 0, 204);
             break;
          }
        case QUEUE_INCOMPLETE:
          {
             httpd_uri_data(huu->module, (char *)s, strlen(s), 206);
             break;
          }
        case QUEUE_COMPLETE:
        default:
          httpd_uri_data(huu->module, (char *)s, strlen(s), 200);
     }

   azy_server_module_events_resume(huu->module, EINA_TRUE);
   free((char *)s);
}

void
httpd_uri_uptime_init(void)
{
   httpd_module_add("/uptime", httpd_uri_uptime, NULL,
                    "This API call will let you retrieve output of "
                    "the uptime command.<br />"
                    "<b>As this command is ran through the spam module, "
                    "you can query multiple bots at once.<br />"
                    "Returned data will be equal to the /spam URI.</b><br />"
                    "<div class=\"panel panel-primary\">"
                    "  <div class=\"panel-heading\">"
                    "     <h3 class=\"panel-title\">Example with pattern</h3>"
                    "  </div>"
                    "  <div class=\"panel-body blackback\">"
                    "    <span class=yellow><b>curl</b></span> <span class=red>-X</span><span class=green>GET</span> <span class=white>http://127.0.0.1:5128</span><span class=cyan>/uptime/</span><span class=green>*backup*</span>"
                    "  </div>"
                    "</div>"
                   );
}

void
httpd_uri_uptime(void *uptime_data EINA_UNUSED,
                 Azy_Server_Module *module,
                 const char *uri,
                 Azy_Net_Data *data EINA_UNUSED)
{
   char *p;
   char **params;
   unsigned int nb;
   Httpd_Uri_Uptime *huu;

   DBG("uri[%s]", uri);

   params = eina_str_split_full(uri, "/", 3, &nb);
   EINA_SAFETY_ON_NULL_GOTO(params, error);

   p = (params[2] && params[2][0]) ? params[2] : "*";
   EINA_SAFETY_ON_TRUE_GOTO(strlen(p) > 200, error_free_params);

   huu = calloc(1, sizeof(Httpd_Uri_Uptime));
   EINA_SAFETY_ON_NULL_GOTO(huu, error_free_params);

   huu->module = module;

   httpd_spam_new(p, ".uptime", ".uptime", httpd_uri_uptime_done, huu);

   azy_server_module_events_suspend(module);

   free(params[0]);
   free(params);

   return;

error_free_params:
   free(params[0]);
   free(params);
error:
   httpd_uri_data(module, NULL, 0, 500);
}

