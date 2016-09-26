#include "log.h"

#define _SETN(_a, _b, _c) eina_value_struct_get(_a, _b, &_c)
#define _SETS(_a, _b, _c)                                                      \
   do {                                                                        \
      char *_d;                                                                \
      eina_value_struct_get(_a, _b, &_d);                                      \
      if(_d) _c = strdup(_d);                                                  \
   } while (0)


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
event_log_last_cb(Esql_Res *res,
                  void *data)
{
   Event_Log_Last *ell = data;
   Eina_Iterator *it;
   const Esql_Row *row;
   Eina_List *l = NULL;
   const char *s;

   s = esql_res_error_get(res);
   IF_TRUE_ERR_SEND_RETURN(!s, ell->citizen, "Failed to fetch last logs : %s", s);

   it = esql_res_row_iterator_new(res);
   EINA_ITERATOR_FOREACH(it, row)
     {
        const Eina_Value *ev;
        Module_Log_Entry *mle;

        ev = esql_row_value_struct_get(row);
        mle = Module_Log_Entry_new();

        _SETN(ev, "id", mle->id);
        _SETS(ev, "date", mle->date);
        _SETN(ev, "type", mle->type);
        _SETS(ev, "source", mle->source);
        _SETS(ev, "data", mle->data);

        l = eina_list_append(l, mle);
     }

   s = gotham_serialize_struct_to_string(l, (Gotham_Serialization_Function)Array_Module_Log_Entry_to_azy_value);
   IF_TRUE_ERR_SEND_RETURN(s, ell->citizen, "Failed to convert esql result to JSON");

   gotham_citizen_send(ell->citizen, s);
   _event_log_last_free(ell);
}

void
event_log_last(void *data,
               Gotham_Citizen_Command *command)
{
   Log *log = data;
   Event_Log_Last *ell;
   unsigned int limit;
   Esql_Query_Id id;

   if (!command->command[2])
     {
        gotham_command_send(command, "Wrong parameters");
        return;
     }

   limit = atoi(command->command[2]);

   ell = _event_log_last_new(log, command->citizen->jid);
   EINA_SAFETY_ON_NULL_RETURN(ell);

   id = esql_query_args(log->bdd.e, ell,
                        "SELECT id, date, type, source, data "
                        "FROM log "
                        "LIMIT 0, %d", limit);
   EINA_SAFETY_ON_TRUE_RETURN(!id);

   esql_query_callback_set(id, event_log_last_cb);

   return;
}

#undef _SETN
#undef _SETS
