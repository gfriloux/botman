#include "httpd.h"

Module_Httpd_Module *
httpd_uri_find(const char *path)
{
   Module_Httpd_Module *mhm;
   Eina_List *l;

   EINA_LIST_FOREACH(_httpd->modules, l, mhm)
     {
DBG("Compare [%s] <-> [%s]", mhm->uri, path);
        if (strncmp(mhm->uri, path, strlen(mhm->uri)))
          continue;

        return mhm;
     }

   return NULL;
}

void
httpd_uri_options(Azy_Server_Module *module,
                  Azy_Net *net)
{
   Azy_Net_Data data;
   Eina_Bool r;

   azy_net_code_set(net, 204);
   azy_net_header_set(net, "Content-Type", "text/plain");

   azy_net_header_set(net, "Access-Control-Allow-Origin", "*");
   azy_net_header_set(net, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
   azy_net_header_set(net, "Access-Control-Allow-Headers", "accept, authorization");

   data.data = NULL;
   data.size = 0;

   r = azy_server_module_send(module, net, &data);
   if (!r) ERR("Error while sending answer !");
}

void
httpd_uri_data(Azy_Server_Module *module,
               char *data,
               size_t len,
               int http_code)
{
   Azy_Net *net;
   Azy_Net_Data and;
   Eina_Bool r;

   net = azy_server_module_net_get(module);
   EINA_SAFETY_ON_NULL_RETURN(net);

   azy_net_transport_set(net, AZY_NET_TRANSPORT_JSON);
   azy_net_code_set(net, http_code);
   azy_net_header_set(net, "Access-Control-Allow-Origin", "*");
   azy_net_header_set(net, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
   azy_net_header_set(net, "Access-Control-Allow-Headers", "accept, authorization");
   if (!len) azy_net_header_set(net, "Content-Length", "0");

   and.data = (unsigned char *) data;
   and.size = len;

   r = azy_server_module_send(module, net, &and);
   if (!r) ERR("Error while sending answer !");

   return;
}

