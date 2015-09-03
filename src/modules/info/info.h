#ifndef ALERT_H
#define ALERT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>
#include <cJSON.h>

#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define MODULE_INFO_CONF SYSCONF_DIR"/gotham/modules.conf.d/info.conf"

int _module_info_log_dom;

typedef struct _Module_Info
{
   Gotham *gotham;
   Eina_Array *search_vars;

   Ecore_Timer *sync;
} Module_Info;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_info_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_info_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_info_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_info_log_dom, __VA_ARGS__)

void info_alfred_command(Module_Info *info, Gotham_Citizen_Command *command);
void info_conf_alfred_load(Module_Info *info);

void info_botman_command(Module_Info *info, Gotham_Citizen_Command *command);
Eina_Bool info_botman_sync(void *data);

void module_json_answer(const char *cmd, const char *params, Eina_Bool status, Eina_Strbuf *content, Gotham *gotham, Gotham_Citizen *citizen, Eina_Bool send_to_alfred);

#endif