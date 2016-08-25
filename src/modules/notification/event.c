#include "main.h"

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
     alfred_group_add(notification, command);
   else if ((command->command[1]) && (!strcmp(command->command[1], "groupdel")))
     alfred_group_del(notification, command);
   else if ((command->command[1]) && (!strcmp(command->command[1], "useradd")))
     alfred_user_add(notification, command);
   else if ((command->command[1]) && (!strcmp(command->command[1], "userdel")))
     alfred_user_del(notification, command);
   else if ((command->command[1]) && (!strcmp(command->command[1], "send")))
     alfred_send(notification, command);
   else
     DBG("WHAT SHOULD I DO ?");

   return EINA_TRUE;
}
