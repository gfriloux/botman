#include <Eina.h>
#include <Ecore.h>
#include <Esskyuehl.h>
#include <Gotham.h>

#define ERR(...) EINA_LOG_DOM_ERR(_module_log_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_log_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_log_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_module_log_log_dom, __VA_ARGS__)
#define CRI(...) EINA_LOG_DOM_CRIT(_module_log_log_dom, __VA_ARGS__)

#define MODULE_LOG_DB DATA_DIR"/gotham/modules.d/log/log.db"

int _module_log_log_dom;

typedef enum _Log_Type
{
   LOG_TYPE_CONNECT,
   LOG_TYPE_DISCONNECT,
   LOG_TYPE_MESSAGE
} Log_Type;

typedef struct _Log
{
   Gotham *gotham;

   struct {
      Esql *e;
      Ecore_Event_Handler *ec,
                          *ed;
      Eina_Bool connected;
   } bdd;
} Log;

Eina_Bool log_esql_connect(void *data, int type, void *ev);
Eina_Bool log_esql_disconnect(void *data, int type, void *ev);
Eina_Bool log_esql_add(Log *log, Log_Type type, const char *source, const char *message);
