#include "network.h"

static const char *name = "Network";

void *
module_register(Gotham *gotham)
{
   Module_Network *network;
#ifdef _WIN32
   int r;
#endif

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        NFO("Cant load for this gotham type");
        return NULL;
     }

   network = calloc(1, sizeof(Module_Network));
   if (!network)
     {
        ERR("Failed to allocate Module_Network structure");
        return NULL;
     }

   network->gotham = gotham;

#ifdef _WIN32
   r = WSAStartup(MAKEWORD(2, 2), &network->wsadata);
   EINA_SAFETY_ON_TRUE_GOTO(r, free_network);
#endif

   gotham_modules_command_add("network", ".network",
                              "[.network] - "
                              "This command will return the actual network "
                              "configuration.");

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

void
module_json_answer(const char *cmd,
                   const char *params,
                   Eina_Bool status,
                   const char *content,
                   Gotham *gotham,
                   const char *jid)
{
   cJSON *json_obj,
         *json_content;
   char **split;
   unsigned int i;
   char *ptr;

   json_obj = cJSON_CreateObject();
   cJSON_AddStringToObject(json_obj, "command", cmd);
   cJSON_AddStringToObject(json_obj, "parameters", params);
   cJSON_AddStringToObject(json_obj, "status", status ? "ok" : "ko");

   json_content = cJSON_CreateArray();
   split = eina_str_split(content, "\n", 0);
   for (i=0; split[i]; i++)
     if (split[i][0])
       cJSON_AddItemToArray(json_content, cJSON_CreateString(split[i]));

   cJSON_AddItemToObject(json_obj, "content", json_content);
   if (split[0]) free(split[0]);
   free(split);

   ptr = cJSON_Print(json_obj);
   cJSON_Delete(json_obj);
   shotgun_message_send(gotham->shotgun, jid, ptr,
                        SHOTGUN_MESSAGE_STATUS_ACTIVE, EINA_TRUE);

   free(ptr);
   return;
}
