#include "network.h"

static const char *name = "Network";

void *
module_register(Gotham *gotham)
{
   Module_Network *network;
#ifdef _WIN32
   int r;
#endif

   EINA_SAFETY_ON_TRUE_RETURN_VAL(gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN, NULL);

   network = calloc(1, sizeof(Module_Network));
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, NULL);

   network->gotham = gotham;
#ifdef _WIN32
   r = WSAStartup(MAKEWORD(2, 2), &network->wsadata);
   EINA_SAFETY_ON_TRUE_GOTO(r, free_network);
#endif
   gotham_modules_command_add("network", ".network",
                              "[.network] - "
                              "This command will return the actual network "
                              "configuration.", NULL);

   return network;

#ifdef _WIN32
free_network:
#endif
   free(network);
   return NULL;
}

void
module_unregister(void *data)
{
   Module_Network *network = data;

   free(network);
#ifdef _WIN32
   WSACleanup();
#endif
   gotham_modules_command_del("network", ".network");
}

const char *
module_init(void)
{
   eina_init();
   _module_network_log_dom = eina_log_domain_register("module_network", EINA_COLOR_RED);

   DBG("Initializing");
   return name;
}

void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_network_log_dom);
   eina_shutdown();
}
