#ifndef ALERT_H
#define ALERT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#include "Module_Common_Azy.h"

#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define MODULE_INFO_CONF SYSCONF_DIR"/gotham/modules.conf.d/info.conf"

int _module_info_log_dom;

typedef struct _Module_Info
{
   Gotham *gotham;
   Module_Info_Conf *conf;
   Ecore_Timer *sync;
} Module_Info;

typedef struct _Alfred_Sort Alfred_Sort;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_info_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_info_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_info_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_info_log_dom, __VA_ARGS__)

Alfred_Sort * alfred_sort_new(Module_Info *info);
void alfred_sort_free(Alfred_Sort *as);
void alfred_sort_add(Alfred_Sort *as, const char *value, Gotham_Citizen *citizen);
char * alfred_sort_print(Alfred_Sort *as, const char *operation);


void info_alfred_command(Module_Info *info, Gotham_Citizen_Command *command);

void info_botman_command(Module_Info *info, Gotham_Citizen_Command *command);
Eina_Bool info_botman_sync(void *data);

const char * utils_citizen_print(Module_Info *info, Gotham_Citizen *citizen);
#endif
