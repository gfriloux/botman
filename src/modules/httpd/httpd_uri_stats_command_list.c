#include "httpd.h"

void
httpd_uri_stats_command_list_init(void)
{
   httpd_module_add("/stats/commands/list", httpd_uri_stats_command_list, NULL,
                    "This API call will let you retrieve the list of API "
                    "calls available through this web service.<br />"
                    "<br />"
                    "<div class=\"panel panel-primary\">"
                    "  <div class=\"panel-heading\">"
                    "     <h3 class=\"panel-title\">Example</h3>"
                    "  </div>"
                    "  <div class=\"panel-body blackback\">"
                    "    <span class=yellow><b>curl</b></span> <span class=red>-X</span><span class=green>GET</span> <span class=white>http://127.0.0.1:5128</span><span class=cyan>/stats/commands/list</span>"
                    "  </div>"
                    "</div>"
                   );
}

void
httpd_uri_stats_command_list(void *stats_data EINA_UNUSED,
                             Azy_Server_Module *module,
                             const char *uri,
                             Azy_Net_Data *data EINA_UNUSED)
{
   cJSON *json,
         *command;
   char *s;
   Eina_List *l;
   Module_Httpd_Module *mhm;

   DBG("uri[%s]", uri);

   json = cJSON_CreateArray();
   EINA_SAFETY_ON_NULL_GOTO(json, error);

   EINA_LIST_FOREACH(_httpd->modules, l, mhm)
     {
        command = cJSON_CreateObject();

        cJSON_AddItemToObject(command, "uri", cJSON_CreateString(mhm->uri));
        cJSON_AddItemToObject(command, "description", cJSON_CreateString(mhm->description));

        cJSON_AddItemToArray(json, command);
     }

   s = cJSON_Print(json);

   cJSON_Delete(json);

   httpd_uri_data(module, s, strlen(s), 200);
   free(s);
   return;

error:
   httpd_uri_data(module, NULL, 0, 500);
}
