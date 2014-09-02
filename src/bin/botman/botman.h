#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_Getopt.h>
#include <Gotham.h>

int botman_log_dom;
Eina_Bool connected;

#define CRI(...) EINA_LOG_DOM_CRIT(botman_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(botman_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(botman_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(botman_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(botman_log_dom, __VA_ARGS__)

Eina_Bool botman_event_connect(void *data, int type, void *ev);
Eina_Bool botman_event_disconnect(void *data, int type, void *ev);
Eina_Bool botman_event_citizen_list(void *data, int type, void *ev);
Eina_Bool botman_event_citizen_connect(void *data, int type, void *ev);
Eina_Bool botman_event_citizen_command(void *data, int type, void *ev);
