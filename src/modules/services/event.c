#include "services.h"

Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Services *services = data;
   Gotham_Citizen_Command *command = ev;

   DBG("services[%p] command[%p]", services, command);

   if (strcmp(command->name, ".service"))
     return EINA_TRUE;

   command->handled = EINA_TRUE;

   if ( (command->command[1]) &&
        (!strcmp(command->command[1], "start")))
     command_start(services, command);
   else if ( (command->command[1]) &&
        (!strcmp(command->command[1], "restart")))
     command_restart(services, command);
   else if ( (command->command[1]) &&
        (!strcmp(command->command[1], "stop")))
     command_stop(services, command);
   else if ( (command->command[1]) &&
        (!strcmp(command->command[1], "status")))
     command_status(services, command);
   /* TODO : Handle unknown param */

   return EINA_TRUE;
}
