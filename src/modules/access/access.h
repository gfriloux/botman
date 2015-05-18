#ifndef ACCESS_H
#define ACCESS_H

#include <Eina.h>
#include <Gotham.h>
#include <cJSON.h>

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

typedef struct _Module_Access_Rule
{
   const char *pattern,       /*!<  Pattern to match                         */
              *description;   /*!<  A short description                      */
   unsigned int level;        /*!<  Access level                             */
} Module_Access_Rule;

typedef struct _Module_Access
{
   Gotham *gotham;            /*!<  Gotham structure                         */
   unsigned int revision;     /*!<  Access conf revision number              */
   Eina_List *citizens,     /*!<  Citizen access list                      */
             *commands;     /*!<  Commands access list                     */
} Module_Access;

void access_conf_load(Module_Access *access);
void access_conf_save(Module_Access *access);
cJSON * access_conf_tojson(Module_Access *access);

Eina_Bool access_allowed(Gotham_Module_Command *mc, Gotham_Citizen *citizen);

void citizen_access_eval(Module_Access *access, Gotham_Citizen *citizen);
const char * citizen_access_list(Module_Access *access);
const char * citizen_access_set(Module_Access *access, Gotham_Citizen_Command *command);
const char * citizen_access_del(Module_Access *access, Gotham_Citizen_Command *command);
const char * citizen_access_add(Module_Access *access, Gotham_Citizen_Command *command);
const char * citizen_access_sync(Module_Access *access);

unsigned int modules_commands_level_find(Module_Access *access, const char *command);
void modules_commands_check(Module_Access *access, Gotham_Module *module);

void alfred_commands_register(void);
void alfred_commands_unregister(void);

void botman_access_sync(Module_Access *access, Gotham_Citizen_Command *command);
void botman_commands_register(void);
void botman_commands_unregister(void);
void botman_access_alfred_add(Module_Access *access);
#endif

/**
 * @}
 */
