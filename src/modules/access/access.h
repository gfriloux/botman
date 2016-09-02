#ifndef ACCESS_H
#define ACCESS_H

#include <Eina.h>
#include <Gotham.h>
#include <cJSON.h>

#include "Module_Common_Azy.h"

#define CRI(...) EINA_LOG_DOM_CRIT(_module_access_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_access_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_access_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_access_log_dom, __VA_ARGS__)

/**
 * @addtogroup Gotham_Module_Access
 * @{
 */

/**
 * Path to acces module conf file
 */
#define MODULE_ACCESS_CONF SYSCONF_DIR"/gotham/modules.conf.d/access.conf"

int _module_access_log_dom;

typedef struct _Module_Access
{
   Gotham *gotham;            /*!<  Gotham structure                         */
   Module_Access_Conf *conf;
} Module_Access;

Eina_Bool access_allowed(Gotham_Module_Command *mc, Gotham_Citizen *citizen);

void citizen_access_eval(Module_Access *access, Gotham_Citizen *citizen);
void citizen_access_list(void *data, Gotham_Citizen_Command *command);
void citizen_access_set(void *data, Gotham_Citizen_Command *command);
void citizen_access_del(void *data, Gotham_Citizen_Command *command);
void citizen_access_add(void *data, Gotham_Citizen_Command *command);
void citizen_access_sync(void *data, Gotham_Citizen_Command *command);

unsigned int modules_commands_level_find(Module_Access *access, const char *command);
void modules_commands_check(Module_Access *access, Gotham_Module *module);

void alfred_commands_register(void);
void alfred_commands_unregister(void);

void botman_access_sync(void *data, Gotham_Citizen_Command *command);
void botman_commands_register(void);
void botman_commands_unregister(void);
void botman_access_alfred_add(Module_Access *access);
#endif

/**
 * @}
 */
