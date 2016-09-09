#include "shutdown.h"

static const char *name = "Shutdown";

void *
module_register(Gotham *gotham)
{
   EINA_SAFETY_ON_TRUE_RETURN_VAL(gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN, NULL);
   gotham_modules_command_add("shutdown", ".reboot",
                              "[.reboot] -"
                              "This command will initiate a system reboot.",
                              NULL);
   return (char *)name;
}

void
module_unregister(void *data EINA_UNUSED)
{
   gotham_modules_command_del("shutdown", ".reboot");
}

const char *
module_init(void)
{
   eina_init();

   _module_shutdown_log_dom = eina_log_domain_register("module_shutdown", EINA_COLOR_YELLOW);
   DBG("Initializing");
   return name;
}

void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_shutdown_log_dom);
   eina_shutdown();
}
