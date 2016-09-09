#include "httpd.h"

typedef struct _Httpd_Uri_Service
{
   Azy_Server_Module *module;
} Httpd_Uri_Service;

void
httpd_uri_service_done(void *data,
                       Httpd_Queue_State state,
                       Eina_List *answers)
{
   Httpd_Uri_Service *hus = data;
   const char *s;

   s = gotham_serialize_struct_to_string(answers, (Gotham_Serialization_Function)Array_Httpd_Spam_Answer_Message_to_azy_value);

   switch (state)
     {
        case QUEUE_NO_RESULTS:
          {
             httpd_uri_data(hus->module, NULL, 0, 204);
             break;
          }
        case QUEUE_INCOMPLETE:
          {
             httpd_uri_data(hus->module, (char *)s, strlen(s), 206);
             break;
          }
        case QUEUE_COMPLETE:
        default:
          httpd_uri_data(hus->module, (char *)s, strlen(s), 200);
     }

   azy_server_module_events_resume(hus->module, EINA_TRUE);
   free((char *)s);
}

void
httpd_uri_service_init(void)
{
   httpd_module_add("/service", httpd_uri_service, NULL,
                    "This API call will give you access to management "
                    "of services.<br />"
                    "<b>As this command is ran through the spam module, "
                    "you can query multiple bots at once.<br />"
                    "Returned data will be equal to the /spam URI.</b><br />"
                    "URI is separated by at least 2 params :"
                    "<ul>"
                    "  <li>Service operation : start, stop, restart, status"
                    "  </li>"
                    "  <li>Service name</li>"
                    "  <li>Optional pattern matching bots to contact</li>"
                    "</ul>"
                    "<div class=\"panel panel-primary\">"
                    "  <div class=\"panel-heading\">"
                    "     <h3 class=\"panel-title\">Example with pattern</h3>"
                    "  </div>"
                    "  <div class=\"panel-body blackback\">"
                    "    <span class=yellow><b>curl</b></span> <span class=red>-X</span><span class=green>GET</span> <span class=white>http://127.0.0.1:5128</span><span class=cyan>/service/<span class=green>start</span><span class=cyan>/</span><span class=green>prosave</span><span class=cyan>/</span><span class=green>*backup*</span>"
                    "  </div>"
                    "</div>"
                    );
}

void
httpd_uri_service(void *service_data EINA_UNUSED,
                  Azy_Server_Module *module,
                  const char *uri,
                  Azy_Net_Data *data EINA_UNUSED)
{
   char *p,
        **params;
   unsigned int nb;
   Eina_Strbuf *buf;
   Httpd_Uri_Service *hus;

   DBG("uri[%s]", uri);

   params = eina_str_split_full(uri, "/", 5, &nb);
   EINA_SAFETY_ON_NULL_GOTO(params, error);

   p = (params[4] && params[4][0]) ? params[4] : "*";
   EINA_SAFETY_ON_TRUE_GOTO(strlen(p) > 200, error_free_params);

   hus = calloc(1, sizeof(Httpd_Uri_Service));
   EINA_SAFETY_ON_NULL_GOTO(hus, error_free_params);

   hus->module = module;

   buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_GOTO(buf, error_free_hus);

   eina_strbuf_append_printf(buf, ".service %s %s", params[2], params[3]);

   httpd_spam_new(p, ".service", eina_strbuf_string_get(buf), httpd_uri_service_done, hus);

   azy_server_module_events_suspend(module);

   eina_strbuf_free(buf);
   free(params[0]);
   free(params);

   return;

error_free_hus:
   free(hus);
error_free_params:
   free(params[0]);
   free(params);
error:
   httpd_uri_data(module, NULL, 0, 500);
}
