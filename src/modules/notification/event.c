#include "main.h"

#define AUTH(_a, _b, _c)                                                       \
{                                                                              \
   if ((_a->access_allowed) && (!_a->access_allowed(_b, _c)))                  \
     {                                                                         \
        ERR("%s is not autorized", _c->jid);                                   \
        Eina_Strbuf *buf = eina_strbuf_new();                                  \
        eina_strbuf_append(buf, "Access denied");                              \
        module_json_answer(".notification", "", EINA_FALSE,                    \
                           buf, _a->gotham, _c, EINA_FALSE);                   \
        eina_strbuf_free(buf);                                                 \
        return EINA_TRUE;                                                      \
     }                                                                         \
}
/* "debug" */

Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Notification *notification = data;
   Gotham_Citizen_Command *command = ev;
   Gotham_Citizen *citizen = command->citizen;

   if (strcmp(command->name, ".notification")) return EINA_TRUE;

   DBG("notification[%p] command[%p][%s] citizen[%p][%s]",
       notification, command, command->name, citizen, citizen->jid);

   command->handled = EINA_TRUE;

   if (!command->command[1]) alfred_group_print(notification, command);
   else if ((command->command[1]) && (!strcmp(command->command[1], "groupadd")))
     {
        AUTH(notification, gotham_modules_command_get(".notification groupadd"), citizen);
        alfred_group_add(notification, command);
     }
   else if ((command->command[1]) && (!strcmp(command->command[1], "groupdel")))
     {
        AUTH(notification, gotham_modules_command_get(".notification groupdel"), citizen);
        alfred_group_del(notification, command);
     }
   else if ((command->command[1]) && (!strcmp(command->command[1], "useradd")))
     {
        AUTH(notification, gotham_modules_command_get(".notification useradd"), citizen);
        alfred_user_add(notification, command);
     }
   else if ((command->command[1]) && (!strcmp(command->command[1], "userdel")))
     {
        AUTH(notification, gotham_modules_command_get(".notification userdel"), citizen);
        alfred_user_del(notification, command);
     }
   else if ((command->command[1]) && (!strcmp(command->command[1], "send")))
     {
        AUTH(notification, gotham_modules_command_get(".notification send"), citizen);
        alfred_send(notification, command);
     }
   else
     DBG("WHAT SHOULD I DO ?");

   return EINA_TRUE;
}
