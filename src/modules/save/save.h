#ifndef SAVE_H
#define SAVE_H
#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>
#include <cJSON.h>

#include "Module_Common_Azy.h"
#include "../../lib/gotham/Gotham_Common.h"

#define MODULE_SAVE_CONF SYSCONF_DIR"/gotham/modules.conf.d/save.conf"
#define MODULE_SAVE_BACKUP DATA_DIR"/gotham/modules.d/save/"

int _module_save_log_dom;

typedef struct _Module_Save
{
   Gotham *gotham;
   Module_Save_Conf *conf;
   Ecore_Timer *et_backup;
} Module_Save;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_save_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_save_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_save_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_save_log_dom, __VA_ARGS__)

Eina_Bool conf_backup(void *data);
Eina_Bool conf_restore(Module_Save *save);

void event_citizen_save_info(void *data, Gotham_Citizen_Command *command);
void event_citizen_save_set(void *data, Gotham_Citizen_Command *command);

int utils_json_number_get(cJSON *json, const char *varname);
const char * utils_json_string_get(cJSON *json, const char *varname);
Eina_Bool utils_isnumber(char c);

void save_conf(void);

#endif
