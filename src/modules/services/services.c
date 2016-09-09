#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "services.h"

static const char *name = "Services";

void *
module_register(Gotham *gotham)
{
   Module_Services *services;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return NULL;

   services = calloc(1, sizeof(Module_Services));
   EINA_SAFETY_ON_NULL_RETURN_VAL(services, NULL);

   DBG("services[%p] gotham[%p] shotgun[%p]",
       services, gotham, gotham->shotgun);

   services->gotham = gotham;

   gotham_modules_command_add("services", ".service start",
                              "[.service start servicename] - "
                              "This command will start a given service.", NULL);
   gotham_modules_command_add("services", ".service stop",
                              "[.service stop servicename] - "
                              "This command will stop a given service.", NULL);
   gotham_modules_command_add("services", ".service restart",
                              "[.service restart servicename] - "
                              "This command will restart a given service.", NULL);
   gotham_modules_command_add("services", ".service status",
                              "[.service status servicename] - "
                              "This command will show status of a given service.", NULL);
#ifndef _WIN32
   services->conf = gotham_serialize_file_to_struct(MODULE_SERVICES_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Services_Conf);
#endif
   return services;
}

void
module_unregister(void *data)
{
   Module_Services *services = data;

   DBG("services[%p]", services);

   free(services);
}

const char *
module_init(void)
{
   eina_init();
   _module_services_log_dom = eina_log_domain_register("module_services",
                                                       EINA_COLOR_RED);
   return name;
}

void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_services_log_dom);
   eina_shutdown();
}
