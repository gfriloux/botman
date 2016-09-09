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
   char *s;
   Eina_List *l,
             *list = NULL;
   Module_Httpd_Module *mhm;

   DBG("uri[%s]", uri);

   EINA_LIST_FOREACH(_httpd->modules, l, mhm)
     {
        Httpd_Module *module;

        module = Httpd_Module_new();

        eina_stringshare_replace(&module->uri, mhm->uri);
        eina_stringshare_replace(&module->description, mhm->description);

        list = eina_list_append(list, module);
     }

   s = gotham_serialize_struct_to_string(list, (Gotham_Serialization_Function)Array_Httpd_Module_to_azy_value);

   httpd_uri_data(module, s, strlen(s), 200);
   free(s);
   Array_Httpd_Module_free(list);
   return;

error:
   httpd_uri_data(module, NULL, 0, 500);
}
