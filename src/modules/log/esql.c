#include "log.h"

void
_log_esql_add_cb(Esql_Res *res,
                 void *data EINA_UNUSED)
{
   const char *s;

   s = esql_res_error_get(res);

   // Should we send a notification ?
   if (s) ERR("Failed to insert log : %s", s);
}

Eina_Bool
log_esql_add(Log *log,
             Log_Type type,
             const char *source,
             const char *message)
{
   Esql_Query_Id id;

   DBG("log[%p] type[%d] source[%s] message[%s]", log, type, source, message);

   EINA_SAFETY_ON_TRUE_RETURN_VAL(!log->bdd.e, EINA_TRUE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!log->bdd.connected, EINA_TRUE);

   id = esql_query_args(log->bdd.e, log,
                        "INSERT INTO log ('type', 'source', 'data') "
                                 "VALUES ('%d', '%s', '%s');",
                        type, source, message);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!id, EINA_TRUE);

   esql_query_callback_set(id, _log_esql_add_cb);

   return EINA_TRUE;
}

Eina_Bool
log_esql_connect(void *data,
                int type EINA_UNUSED,
                void *ev)
{
   Log *log = data;
   Esql *e = ev;
   Esql_Query_Id id;

   if ((data != esql_data_get(e)) || (!data)) return EINA_TRUE;

   DBG("log[%p]", log);
   log->bdd.connected = EINA_TRUE;

   id = esql_query_args(log->bdd.e, log,
                        "CREATE TABLE IF NOT EXISTS log("
                           "id INTEGER PRIMARY KEY,"
                           "date DATE DEFAULT (datetime('now', 'localtime')),"
                           "type INTEGER,"
                           "source TEXT NOT NULL,"
                           "data TEXT"
                        ");");
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!id, EINA_TRUE);
   return EINA_TRUE;
}

Eina_Bool
log_esql_disconnect(void *data,
                   int type EINA_UNUSED,
                   void *ev)
{
   Log *log = data;
   Esql *e = ev;

   if ((data != esql_data_get(e)) || (!data)) return EINA_TRUE;

   DBG("log[%p]", log);
   log->bdd.connected = EINA_FALSE;
   return EINA_TRUE;
}
