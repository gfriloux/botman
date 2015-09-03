#ifndef REWRITE_H
#define REWRITE_H

#include <Eina.h>
#include <Gotham.h>
#include <cJSON.h>

#define MODULE_REWRITE_CONF SYSCONF_DIR"/gotham/modules.conf.d/rewrite.conf"
int _module_rewrite_log_dom;

typedef struct _Module_Rewrite
{
   Gotham *gotham;
   Eina_Inlist *rules;
   Eina_Hash *rw;
   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
} Module_Rewrite;

typedef struct _Module_Rewrite_Rule
{
   EINA_INLIST;
   const char *name,
              *filter,
              *rule,
              *desc;
} Module_Rewrite_Rule;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_rewrite_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_rewrite_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_rewrite_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_rewrite_log_dom, __VA_ARGS__)

void conf_load(Module_Rewrite *rewrite);
Eina_Bool events_citizen_command(void *data, int type, void *ev);
char * utils_escape(const char *str);
const char * utils_rewrite(Module_Rewrite_Rule *rule, const char *s);
char * utils_strdupargs(const char *s, ...);
#endif
