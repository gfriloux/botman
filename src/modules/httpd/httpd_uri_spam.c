#include "httpd.h"

typedef struct _Httpd_Uri_Spam
{
   Azy_Server_Module *module;
} Httpd_Uri_Spam;

void
httpd_uri_spam_init(void)
{
   httpd_module_add("/spam", httpd_uri_spam, NULL,
                    "This API call will let you send commands to multiple bots"
                    " at once.<br />"
                    "<div class=\"panel panel-primary\">"
                    "  <div class=\"panel-heading\">"
                    "     <h3 class=\"panel-title\">Example with pattern</h3>"
                    "  </div>"
                    "  <div class=\"panel-body blackback\">"
                    "     <span class=yellow><b>curl</b></span> <span class=red>-d</span> <span class=green>'.command'</span> <span class=white>http://127.0.0.1:5128</span><span class=cyan>/spam/</span><span class=green>*</span>"
                    "  </div>"
                    "</div>"
                    "<h3>Possible returned HTTP codes</h3>"
                    "  <ul>"
                    "    <li>200 : Every queried bots answered.</li>"
                    "    <li><span class=purple>"
                    "        204 : Your given pattern did not match any bots (no data will be sent)."
                    "    </span></li>"
                    "    <li><span class=goldenrod>"
                    "        206 : Some (or all) bots did not answered."
                    "        In this case, bots that did not answered will "
                    "        have a 0 length message in the returned JSON."
                    "    </span></li>"
                    "  </ul>"
                    "<h3>Returned data</h3>"
                    "  Data returned will be in JSON format.<br />"
                    "  It will be an array of answers from bots.<br />"
                    "  <pre>"
                    "  [<br />"
                    "    {<br />"
                    "      <span class=red>\"jid\"</span> : <span class=red>\"botman-test@xmpp-server.lan\"</span>,<br />"
                    "      <span class=red>\"message\"</span> : <span class=red>\"Data returned by botman-test\"</span><br />"
                    "    }<br />"
                    "  ]<br />"
                    "  </pre>"
                   );

   return;
}

char *
_httpd_uri_spam_command_extract(const char *s,
                                size_t l)
{
   char *c;
   unsigned int i;

   c = calloc(1, l+1);

   for (i = 0; i <= l; i++)
     {
        if (s[i] == ' ')
          break;
     }

   strncpy(c, s, i);
   return c;
}

char *
_httpd_uri_spam_message_extract(const char *s,
                                size_t l)
{
   char *m;

   m = calloc(1, l+1);

   strncpy(m, s, l);

   return m;
}

void
httpd_uri_spam_done(void *data,
                    Httpd_Queue_State state,
                    Eina_List *answers)
{
   Httpd_Uri_Spam *hus = data;
   const char *s;

   s = httpd_uri_spam_answer_to_json(answers);

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
   free(hus);
}

void
httpd_uri_spam(void *spam_data EINA_UNUSED,
               Azy_Server_Module *module,
               const char *uri,
               Azy_Net_Data *data)
{
   char **params,
        *p,
        *m,
        *c;
   Httpd_Uri_Spam *hus;
   unsigned int nb;

   EINA_SAFETY_ON_TRUE_GOTO(((!data) || (!data->size)), bad_request);

   params = eina_str_split_full(uri, "/", 3, &nb);
   EINA_SAFETY_ON_NULL_GOTO(params, error);

   p = (params[2] && params[2][0]) ? params[2] : "*";
   EINA_SAFETY_ON_TRUE_GOTO(strlen(p) > 200, error_free_params);

   hus = calloc(1, sizeof(Httpd_Uri_Spam));
   EINA_SAFETY_ON_NULL_GOTO(hus, error_free_params);

   hus->module = module;

   c = _httpd_uri_spam_command_extract((const char *)data->data, data->size);
   m = _httpd_uri_spam_message_extract((const char *)data->data, data->size);

   DBG("c[%s] m[%s]", c, m);

   httpd_spam_new(p, c, m, httpd_uri_spam_done, hus);

   free(c);
   free(m);

   azy_server_module_events_suspend(module);

   free(params[0]);
   free(params);

   return;

error_free_params:
   free(params[0]);
   free(params);
error:
   httpd_uri_data(module, NULL, 0, 500);
   return;

bad_request:
   httpd_uri_data(module, NULL, 0, 400);
   return;
}
