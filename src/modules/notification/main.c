#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "main.h"

static const char *name = "Notification";

const char *
module_init(void)
{
   eina_init();
   _module_notification_log_dom = eina_log_domain_register("module_notification", EINA_COLOR_RED);
   return name;
}

void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_notification_log_dom);
   eina_shutdown();
}

void *
module_register(Gotham *gotham)
{
   Module_Notification *notification;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED) return NULL;

   notification = calloc(1, sizeof(Module_Notification));
   if (!notification)
     {
        ERR("Failed to allocate notification structure");
        return NULL;
     }

   notification->gotham = gotham;

   notification->conf = gotham_serialize_file_to_struct(MODULE_NOTIFICATION_CONF,
                                                        (Gotham_Deserialization_Function)azy_value_to_Module_Notification_Conf);
   if (!notification->conf)
     {
        notification->conf = Module_Notification_Conf_new();
        EINA_SAFETY_ON_NULL_GOTO(notification->conf, free_notification);
     }

   gotham_modules_command_add("notification", ".notification groupadd",
                              "[.notification groupadd groupname] - "
                              "This command allows you to create a new "
                              "group.");

   gotham_modules_command_add("notification", ".notification groupdel",
                              "[.notification groupdel groupname] - "
                              "This command allows you to delete a group.");

   gotham_modules_command_add("notification", ".notification useradd",
                              "[.notification useradd groupname username] - "
                              "This command allows you to add a given JID "
                              "into a group.");
   return notification;

free_notification:
   free(notification);
   return NULL;
}

void
module_unregister(void *data)
{
   Module_Notification *notification = data;

   gotham_modules_command_del("notification", ".notification groupadd");
   gotham_modules_command_del("notification", ".notification groupdel");
   gotham_modules_command_del("notification", ".notification addtogroup");

   Module_Notification_Conf_free(notification->conf);
   free(notification);
}
