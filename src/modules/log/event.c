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

   Module_Log_Query *mlq;
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
   Module_Log_Query_free(ell->mlq);
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
event_log_total_count_cb(Esql_Res *res,
                         void *data)
{
   Event_Log_Last *ell = data;
   Eina_Iterator *it;
   const Esql_Row *row;
   const char *s;

   s = esql_res_error_get(res);
   IF_TRUE_ERR_SEND_RETURN(!s, ell->citizen, "Failed to fetch last logs : %s", s);

   it = esql_res_row_iterator_new(res);
   EINA_ITERATOR_FOREACH(it, row)
     {
        _SETN_FROM_ROW(row, 0, ell->mlq->total_results);
     }

   s = gotham_serialize_struct_to_string(ell->mlq, (Gotham_Serialization_Function)Module_Log_Query_to_azy_value);
   IF_TRUE_ERR_SEND_RETURN(s, ell->citizen, "Failed to convert esql result to JSON");
   gotham_citizen_send(ell->citizen, s);
   _event_log_last_free(ell);
}

void
event_log_last_cb(Esql_Res *res,
                  void *data)
{
   Event_Log_Last *ell = data;
   Eina_Iterator *it;
   const Esql_Row *row;
   const char *s;
   Esql_Query_Id id;

   s = esql_res_error_get(res);
   IF_TRUE_ERR_SEND_RETURN(!s, ell->citizen, "Failed to fetch last logs : %s", s);

   ell->mlq = Module_Log_Query_new();
   IF_TRUE_ERR_SEND_RETURN(ell->mlq, ell->citizen, "Failed to allocate Module_Log_Query structure");

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

        ell->mlq->results = eina_list_append(ell->mlq->results, mle);
     }

   id = esql_query_args(ell->log->bdd.e, ell,
                        "SELECT count(*) FROM log "
                        "ORDER BY id DESC;");
   EINA_SAFETY_ON_TRUE_RETURN(!id);
   esql_query_callback_set(id, event_log_total_count_cb);
   return;
}

void
event_log_last(void *data,
               Gotham_Citizen_Command *command)
{
   Log *log = data;
   Event_Log_Last *ell;
   unsigned int limit,
                page = 0;
   Esql_Query_Id id;

   if (!command->command[2])
     {
        gotham_command_send(command, "Wrong parameters");
        return;
     }

   limit = atoi(command->command[2]);

   if (command->command[3]) page = atoi(command->command[3]);
   if (page) page--;

   ell = _event_log_last_new(log, command->citizen->jid);
   EINA_SAFETY_ON_NULL_RETURN(ell);

   id = esql_query_args(log->bdd.e, ell,
                        "SELECT id, date, type, source, data "
                        "FROM log "
                        "ORDER BY id DESC "
                        "LIMIT %d, %d", limit * page, limit);
   EINA_SAFETY_ON_TRUE_RETURN(!id);
   esql_query_callback_set(id, event_log_last_cb);
   return;
}

#undef _SETN
#undef _SETS
