#include "httpd.h"

static const char *name = "HTTPd";

void *
module_register(Gotham *gotham)
{
   Module_Httpd *httpd;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED)
     return NULL;

   httpd = calloc(1, sizeof(Module_Httpd));
   EINA_SAFETY_ON_NULL_RETURN_VAL(httpd, NULL);

   DBG("httpd[%p] gotham[%p] shotgun[%p]", httpd, gotham, gotham->shotgun);

   httpd->gotham = gotham;

   httpd->azy = azy_server_new(EINA_FALSE);
   EINA_SAFETY_ON_NULL_GOTO(httpd->azy, free_httpd);

   azy_server_port_set(httpd->azy, 5128);
   azy_server_addr_set(httpd->azy, "0.0.0.0");
   azy_server_module_add(httpd->azy, Httpd_Serve_module_def());

   _httpd = httpd;

   httpd_queue_init();

   return httpd;

free_httpd:
   free(httpd);
   return NULL;
}

void
module_unregister(void *data)
{
   Module_Httpd *httpd = data;
   Module_Httpd_Module *mhm;

   DBG("httpd[%p]", httpd);

   azy_server_free(httpd->azy);

   EINA_LIST_FREE(httpd->modules, mhm)
     httpd_module_free(mhm);
   free(httpd);
}

const char *
module_init(void)
{
   eina_init();
   _module_httpd_log_dom = eina_log_domain_register("module_httpd", EINA_COLOR_RED);
   azy_init();

   DBG("Initializing");

   return name;
}

void
module_shutdown(void)
{
   azy_shutdown();
   eina_log_domain_unregister(_module_httpd_log_dom);
   eina_shutdown();
}
