#ifndef SEEN_H
#define SEEN_H
#include <Eina.h>
#include <Gotham.h>

#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define MODULE_SEEN_CONF SYSCONF_DIR"/gotham/modules.conf.d/seen.conf"

int _module_seen_log_dom;

typedef struct _Module_Seen
{
   Gotham *gotham;
   Eina_Array *vars;
   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
} Module_Seen;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_seen_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_seen_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_seen_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_seen_log_dom, __VA_ARGS__)

char * seen_utils_elapsed_time(double timestamp);
void seen_conf_load(Module_Seen *seen);
const char * seen_query(Module_Seen *seen, const char *pattern);
#endif
