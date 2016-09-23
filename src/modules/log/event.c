#include "log.h"

Eina_Bool
event_connect(void *data,
              int type EINA_UNUSED,
              void *ev EINA_UNUSED)
{
   Log *log = data;

   log_esql_add(log, LOG_TYPE_CONNECT, log->gotham->me->jid, NULL);
   return EINA_TRUE;
}

Eina_Bool
event_disconnect(void *data,
                 int type EINA_UNUSED,
                 void *ev EINA_UNUSED)
{
   Log *log = data;

   log_esql_add(log, LOG_TYPE_DISCONNECT, log->gotham->me->jid, NULL);
   return EINA_TRUE;
}

Eina_Bool
event_citizen_connect(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Log *log = data;
   Gotham_Citizen *citizen = ev;

   log_esql_add(log, LOG_TYPE_CONNECT, citizen->jid, NULL);
   return EINA_TRUE;
}

Eina_Bool
event_citizen_disconnect(void *data,
                         int type EINA_UNUSED,
                         void *ev)
{
   Log *log = data;
   Gotham_Citizen *citizen = ev;

   log_esql_add(log, LOG_TYPE_DISCONNECT, citizen->jid, NULL);
   return EINA_TRUE;
}

Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Log *log = data;
   Gotham_Citizen_Command *command = ev;

   log_esql_add(log, LOG_TYPE_MESSAGE, command->citizen->jid, command->message);
   return EINA_TRUE;
}
