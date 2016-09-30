#include "httpd.h"

typedef void (*Log_Query_Done)(void *data, const char *result);
typedef void (*Log_Query_Error)(void *data, const char *error);
typedef Eina_Bool (*Log_Query)(void *log, Eina_List *filters, unsigned int page, unsigned int limit, Log_Query_Done done_cb, Log_Query_Error error_cb, void *data);

typedef struct _Httpd_Uri_Log
{
   Log_Query func;
   void *data;
} Httpd_Uri_Log;

typedef struct _Httpd_Uri_Log_Query
{
   Httpd_Uri_Log *hul;
   Azy_Server_Module *module;
} Httpd_Uri_Log_Query;

void
_httpd_uri_log_done(void *data,
                    const char *result)
{
   Httpd_Uri_Log_Query *hulq = data;

   httpd_uri_data(hulq->module, (char *)result, strlen(result), 200);
   azy_server_module_events_resume(hulq->module, EINA_TRUE);
   free(hulq);
}

void
_httpd_uri_log_error(void *data,
                     const char *error)
{
   Httpd_Uri_Log_Query *hulq = data;

   ERR("Failed to fetch logs : %s", error);
   httpd_uri_data(hulq->module, NULL, 0, 500);
   azy_server_module_events_resume(hulq->module, EINA_TRUE);
   free(hulq);
}

void
httpd_uri_log(void *log_data,
              Azy_Server_Module *module,
              const char *uri,
              Azy_Net_Data *data EINA_UNUSED)
{
   Httpd_Uri_Log *hul = log_data;
   Httpd_Uri_Log_Query *hulq;
   unsigned int limit = 5,
                page = 1,
                nb;
   char **params;
   Eina_Bool r;

   params = eina_str_split_full(uri, "/", 5, &nb);
   EINA_SAFETY_ON_NULL_GOTO(params, error);

   if (nb > 3) limit = atoi(params[3]);
   if (nb > 4) page = atoi(params[4]);
   free(params[0]);
   free(params);

   hulq = calloc(1, sizeof(Httpd_Uri_Log_Query));
   EINA_SAFETY_ON_NULL_GOTO(hulq, error);

   hulq->hul = hul;
   hulq->module = module;

   r = hul->func(hul->data, NULL, page, limit, _httpd_uri_log_done,
                 _httpd_uri_log_error, hulq);
   EINA_SAFETY_ON_TRUE_GOTO(!r, error);

   azy_server_module_events_suspend(module);
   return;

error:
   httpd_uri_data(module, NULL, 0, 500);
}

void
httpd_uri_log_init(void)
{
   Httpd_Uri_Log *hul;

   hul = calloc(1, sizeof(Httpd_Uri_Log));
   EINA_SAFETY_ON_NULL_RETURN(hul);

   hul->func = gotham_modules_function_get("Log", "event_log_query");
   EINA_SAFETY_ON_NULL_GOTO(hul->func, free_hul);

   hul->data = gotham_modules_data_get("Log");
   DBG("hul->func[%p] hul->data[%p]", hul->func, hul->data);

   httpd_module_add("/log/last", httpd_uri_log, hul,
                    "This command allows you to query logs sorted "
                    "by insertion order (newest first).<br />"
                    "It is possible to navigate through logs, following "
                    "the concept of log pages."
                    "<div class=\"panel panel-primary\">"
                    "  <div class=\"panel-heading\">"
                    "     <h3 class=\"panel-title\">Ask for the last <span class=green>5</span> logs.</h3>"
                    "  </div>"
                    "  <div class=\"panel-body blackback\">"
                    "    <span class=yellow><b>curl</b></span> <span class=white>http://127.0.0.1:5128</span><span class=cyan>/log/last/</span><span class=green>5</span>"
                    "  </div>"
                    "</div>"
                    "<br />"
                    "<div class=\"panel panel-primary\">"
                    "  <div class=\"panel-heading\">"
                    "     <h3 class=\"panel-title\">Ask for the <span class=purple>2</span>nd page of logs, following <span class=green>5</span> logs per page.</h3>"
                    "  </div>"
                    "  <div class=\"panel-body blackback\">"
                    "    <span class=yellow><b>curl</b></span> <span class=white>http://127.0.0.1:5128</span><span class=cyan>/log/last/</span><span class=green>5</span><span class=cyan>/</span><span class=purple>2</span>"
                    "  </div>"
                    "</div>"
                    "<br />");
   return;

free_hul:
   free(hul);
}
