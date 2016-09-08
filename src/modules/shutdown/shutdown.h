#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#ifdef _WIN32
#include <windows.h>
#endif

int _module_shutdown_log_dom;

#define _MSG(_a, _b, _c)                                                       \
   do {                                                                        \
      if (_a->citizen->type == GOTHAM_CITIZEN_TYPE_ALFRED)                     \
        module_json_answer(".reboot", _b, EINA_TRUE, _c,                       \
                           _a->citizen->gotham, _a->citizen->jid);             \
      else gotham_command_send(_a, _c);                                        \
   } while(0)
/* "debug" */

#define CRI(...) EINA_LOG_DOM_CRIT(_module_shutdown_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_shutdown_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_shutdown_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_shutdown_log_dom, __VA_ARGS__)

void module_json_answer(const char *cmd, const char *params, Eina_Bool status, const char *content, Gotham *gotham, const char *jid);

Eina_Bool event_citizen_command(void *data, int type, void *ev);
