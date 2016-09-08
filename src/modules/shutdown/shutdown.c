#include "shutdown.h"

static const char *name = "Shutdown";

void *
module_register(Gotham *gotham)
{
   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        NFO("Cant load for this gotham type");
        return NULL;
     }

   gotham_modules_command_add("shutdown", ".reboot",
                              "[.reboot] -"
                              "This command will initiate a system reboot.");
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

