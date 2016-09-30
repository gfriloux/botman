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

typedef struct _Event_Log_Query
{
   Log *log;
   Module_Log_Query *mlq;

   struct
   {
      Event_Log_Query_Cb done;
      Event_Log_Query_Error_Cb error;
      void *data;
   } cb;
} Event_Log_Query;

Event_Log_Query *
_event_log_query_new(Log *log,
                     Event_Log_Query_Cb done_cb,
                     Event_Log_Query_Error_Cb error_cb,
                     void *data)
{
   Event_Log_Query *elq;

   elq = calloc(1, sizeof(Event_Log_Query));

   elq->log = log;
   elq->mlq = Module_Log_Query_new();

   elq->cb.done = done_cb;
   elq->cb.error = error_cb;
   elq->cb.data = data;

   return elq;
}

void
_event_log_query_free(Event_Log_Query *elq)
{
   Module_Log_Query_free(elq->mlq);
   free(elq);
}

void
event_log_query_total_count_cb(Esql_Res *res,
                               void *data)
{
   Event_Log_Query *elq = data;
   Eina_Iterator *it;
   const Esql_Row *row;
   const char *s;

   s = esql_res_error_get(res);
   if (s)
     {
        ERR("Failed to fetch last logs : %s", s);
        elq->cb.error(elq->cb.data, "Failed to fetch logs");
        _event_log_query_free(elq);
     }

   it = esql_res_row_iterator_new(res);
   EINA_ITERATOR_FOREACH(it, row)
     {
        _SETN_FROM_ROW(row, 0, elq->mlq->total_results);
     }

   s = gotham_serialize_struct_to_string(elq->mlq, (Gotham_Serialization_Function)Module_Log_Query_to_azy_value);
   if (!s)
     {
        ERR("Failed to fetch last logs : %s", s);
        elq->cb.error(elq->cb.data, "Failed to fetch logs");
        _event_log_query_free(elq);
     }
   elq->cb.done(elq->cb.data, s);
   _event_log_query_free(elq);
   free((char *)s);
}

void
event_log_query_cb(Esql_Res *res,
                   void *data)
{
   Event_Log_Query *elq = data;
   Eina_Iterator *it;
   const Esql_Row *row;
   const char *s;
   Esql_Query_Id id;

   s = esql_res_error_get(res);
   if (s)
     {
        ERR("Failed to fetch last logs : %s", s);
        elq->cb.error(elq->cb.data, "Failed to fetch logs");
        _event_log_query_free(elq);
     }

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

        elq->mlq->results = eina_list_append(elq->mlq->results, mle);
     }

   id = esql_query_args(elq->log->bdd.e, elq, "SELECT count(*) FROM log;");
   EINA_SAFETY_ON_TRUE_RETURN(!id);
   esql_query_callback_set(id, event_log_query_total_count_cb);
   return;
}

Eina_Bool
event_log_query(Log *log,
                Eina_List *filters,
                unsigned int page,
                unsigned int limit,
                Event_Log_Query_Cb done_cb,
                Event_Log_Query_Error_Cb error_cb,
                void *data)
{
   Event_Log_Query *elq;
   Esql_Query_Id id;
   Eina_Strbuf *buf;
   Eina_List *l;
   char *s;
   unsigned int i = 0;

   elq = _event_log_query_new(log, done_cb, error_cb, data);
   EINA_SAFETY_ON_NULL_RETURN_VAL(elq, EINA_FALSE);

   elq->mlq->filters = Array_string_copy(filters);
   elq->mlq->page = page;
   elq->mlq->limit = limit;

   buf = eina_strbuf_new();

   if (filters) eina_strbuf_append(buf, "WHERE ");
   EINA_LIST_FOREACH(filters, l, s)
     {
        if (i++ > 0) eina_strbuf_append(buf, "AND ");
        eina_strbuf_append_printf(buf, "%s ", s);
     }
   id = esql_query_args(log->bdd.e, elq,
                        "SELECT id, date, type, source, data "
                        "FROM log "
                        "%s"
                        "ORDER BY id DESC "
                        "LIMIT %d, %d",
                        eina_strbuf_string_get(buf),
                        limit * (page-1), limit);
   eina_strbuf_free(buf);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!id, EINA_FALSE);
   esql_query_callback_set(id, event_log_query_cb);
   return EINA_TRUE;
}
