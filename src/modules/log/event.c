#include "log.h"

#define _SETN(_a, _b, _c) eina_value_struct_get(_a, _b, &_c)
#define _SETS(_a, _b, _c)                                                      \
   do {                                                                        \
      char *_d;                                                                \
      eina_value_struct_get(_a, _b, &_d);                                      \
      if(_d) _c = eina_stringshare_add(_d);                                    \
   } while (0)
#define _SETN_FROM_ROW(_a, _b, _c)                                             \
   {                                                                           \
      Eina_Value val;                                                          \
      char *tmp;                                                               \
      esql_row_value_column_get(_a, _b, &val);                                 \
      tmp = eina_value_to_string(&val);                                        \
      _c = atoll(tmp);                                                         \
      free(tmp);                                                               \
      eina_value_flush(&val);                                                  \
   }

typedef struct _Event_Log_Last
{
   Log *log;
   Gotham_Citizen *citizen;
} Event_Log_Last;

Event_Log_Last *
_event_log_last_new(Log *log,
                    const char *jid)
{
   Event_Log_Last *ell;

   ell = calloc(1, sizeof(Event_Log_Last));

   ell->log = log;
   ell->citizen = gotham_citizen_new(log->gotham, jid);

   return ell;
}

void
_event_log_last_free(Event_Log_Last *ell)
{
   gotham_citizen_free(ell->citizen);
   free(ell);
}

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

void
event_log_last_done(void *data,
                    const char *result)
{
   Event_Log_Last *ell = data;
   DBG("Query result : %s", result);

   gotham_citizen_send(ell->citizen, result);
   _event_log_last_free(ell);
}

void
event_log_last_error(void *data,
                     const char *error)
{
   Event_Log_Last *ell = data;
   ERR("Query failed : %s", error);

   gotham_citizen_send(ell->citizen, error);
   _event_log_last_free(ell);
}

void
event_log_last(void *data,
               Gotham_Citizen_Command *command)
{
   Log *log = data;
   Event_Log_Last *ell;
   unsigned int limit,
                page = 0;
   Eina_Bool r;

   if (!command->command[2])
     {
        gotham_command_send(command, "Wrong parameters");
        return;
     }

   limit = atoi(command->command[2]);

   if (command->command[3]) page = atoi(command->command[3]);

   ell = _event_log_last_new(log, command->citizen->jid);
   EINA_SAFETY_ON_NULL_RETURN(ell);

   r = event_log_query(log, NULL, page, limit,
                       event_log_last_done,
                       event_log_last_error,
                       ell);
   EINA_SAFETY_ON_TRUE_RETURN(!r);
   return;
}

#undef _SETN
#undef _SETS
