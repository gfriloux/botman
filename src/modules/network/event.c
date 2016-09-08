#include "network.h"

Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Network *network = data;
   Gotham_Citizen_Command *command =ev;

   EINA_SAFETY_ON_NULL_RETURN_VAL(network, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   DBG("network[%p] command[%p][%s]", network, command, command->name);

   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if (strcmp(command->name, ".network"))
     return EINA_TRUE;

   command->handled = EINA_TRUE;

#ifdef _WIN32
   network_get_win32(network, command);
#else
   _MSG(command, "", "This platform is not supported by the network module");
#endif

   return EINA_TRUE;
}
