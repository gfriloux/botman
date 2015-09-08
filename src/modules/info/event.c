#include "info.h"

Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Info *info = data;
   Gotham_Citizen_Command *command = ev;
DBG("command->name[%s] command->command[1][%s]", command->name, command->command);
   if (strcmp(command->name, ".info"))
     return EINA_TRUE;

   DBG("info[%p] command[%p]=%s info->gotham->me->type[%d]",
       info, command, command->name, info->gotham->me->type);

   command->handled = EINA_TRUE;

   if (info->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     info_alfred_command(info, command);
   else info_botman_command(info, command);

   return EINA_TRUE;
}

