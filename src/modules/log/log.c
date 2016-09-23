#include "log.h"

static const char *name = "Log";

const char *
module_init(void)
{
   eina_init();
   ecore_init();
   _module_log_log_dom = eina_log_domain_register("module_log", EINA_COLOR_YELLOW);
   esql_init();
   return name;
}

void *
module_register(Gotham *gotham)
{
   Log *log;
   Eina_Bool r;

   log = calloc(1, sizeof(Log));
   EINA_SAFETY_ON_NULL_RETURN_VAL(log, NULL);

   log->gotham = gotham;

   log->bdd.e = esql_new(ESQL_TYPE_SQLITE);
   EINA_SAFETY_ON_NULL_GOTO(log->bdd.e, free_log);

#define _EV(_a, _b, _c, _d, _e)                                               \
   do {                                                                       \
      _a = ecore_event_handler_add(ESQL_EVENT_##_b, log_esql_##_c, _d);       \
      EINA_SAFETY_ON_NULL_GOTO(_a, _e);                                       \
   } while(0)
   _EV(log->bdd.ec, CONNECT, connect, log, free_e);
   _EV(log->bdd.ed, DISCONNECT, disconnect, log, free_connect);
#undef _EV

   esql_data_set(log->bdd.e, log);

   r = esql_connect(log->bdd.e, MODULE_LOG_DB, NULL, NULL);
   EINA_SAFETY_ON_TRUE_GOTO(!r, free_disconnect);

   return log;

free_disconnect:
   ecore_event_handler_del(log->bdd.ed);
free_connect:
   ecore_event_handler_del(log->bdd.ec);
free_e:
   esql_free(log->bdd.e);
free_log:
   free(log);
   return NULL;
}

void
module_unregister(void *data)
{
   Log *log = data;

   ecore_event_handler_del(log->bdd.ec);
   ecore_event_handler_del(log->bdd.ed);
   esql_free(log->bdd.e);
   free(log);
}

void
module_shutdown(void)
{
   esql_shutdown();
   eina_log_domain_unregister(_module_log_log_dom);
   ecore_shutdown();
   eina_shutdown();
}
