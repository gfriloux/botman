#include <Eina.h>
#include <Ecore.h>
#include <Esskyuehl.h>
#include <Gotham.h>

#include "Module_Common_Azy.h"

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

#define SEND_STR(_a, ...)                                                      \
   do {                                                                        \
      Eina_Strbuf *buf = eina_strbuf_new();                                    \
      eina_strbuf_append_printf(buf, __VA_ARGS__);                             \
      gotham_citizen_send(_a, eina_strbuf_string_get(buf));                    \
      eina_strbuf_free(buf);                                                   \
   } while (0)

#define IF_TRUE_ERR_SEND_RETURN(_a, _b, ...)                                   \
   do {                                                                        \
      if (_a) break;                                                           \
      ERR(__VA_ARGS__);                                                        \
      SEND_STR(_b, __VA_ARGS__);                                               \
      return;                                                                  \
   } while (0)

typedef void (*Event_Log_Query_Cb)(void *data, const char *result);
typedef void (*Event_Log_Query_Error_Cb)(void *data, const char *error);

void event_log_last(void *data, Gotham_Citizen_Command *command);

Eina_Bool event_log_query(Log *log, Eina_List *filters, unsigned int page, unsigned int limit, Event_Log_Query_Cb done_cb, Event_Log_Query_Error_Cb error_cb, void *data);

Eina_Bool log_esql_connect(void *data, int type, void *ev);
Eina_Bool log_esql_disconnect(void *data, int type, void *ev);
Eina_Bool log_esql_add(Log *log, Log_Type type, const char *source, const char *message);
