#include "httpd.h"

Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Httpd *httpd = data;

   DBG("httpd[%p] : Modules loaded, grabbing functions", httpd);

   httpd_spam_init();
   httpd_uri_stats_command_list_init();
   httpd_uri_seen_init();
   httpd_uri_spam_init();
   httpd_uri_uptime_init();
   httpd_uri_service_init();
   httpd_uri_network_init();
   httpd_uri_log_init();

   httpd_uri_stats_init();

   azy_server_start(httpd->azy);

   return EINA_TRUE;
}

Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Httpd *httpd = data;
   Gotham_Citizen_Command *command = ev;

   DBG("httpd[%p] : Received message", httpd);

   DBG("command->name[%s] command->jid[%s] command->message[%s]",
       command->name, command->jid, command->message);

   /* Looking if received message is wanted by our queue */
   httpd_queue_perform(command);

   /* Logging command into our queue.
    * We do not want to log commands about botmans.
    */
   if (command->citizen->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        httpd_uri_stats_command_add(command);
     }

   return EINA_TRUE;
}
