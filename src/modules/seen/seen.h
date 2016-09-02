#ifndef SEEN_H
#define SEEN_H
#include <Eina.h>
#include <Gotham.h>

#include "Module_Common_Azy.h"

#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define MODULE_SEEN_CONF SYSCONF_DIR"/gotham/modules.conf.d/seen.conf"

int _module_seen_log_dom;

typedef struct _Module_Seen
{
   Gotham *gotham;
   Module_Seen_Conf *conf;
} Module_Seen;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_seen_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_seen_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_seen_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_seen_log_dom, __VA_ARGS__)

void event_command_seen(void *data, Gotham_Citizen_Command *command);
char * seen_utils_elapsed_time(double timestamp);
void seen_conf_load(Module_Seen *seen);
const char * seen_query(Module_Seen *seen, const char *pattern);
Eina_List * seen_query_match(Gotham *gotham, const char *pattern, Gotham_Citizen_Type citizen_type, Module_Seen *seen);
#endif
