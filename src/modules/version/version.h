#ifndef VERSION_H
#define VERSION_H
#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#include "Module_Common_Azy.h"

#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define MODULE_VERSION_CONF SYSCONF_DIR"/gotham/modules.conf.d/version.conf"

int _module_version_log_dom;

typedef enum _Module_Version_Sign
{
   MODULE_VERSION_SIGN_UNIDENTIFIED = 0,
   MODULE_VERSION_SIGN_DIFF = 1,
   MODULE_VERSION_SIGN_LT = 2,
   MODULE_VERSION_SIGN_EQ = 4,
   MODULE_VERSION_SIGN_GT = 8
} Module_Version_Sign;

typedef struct _Module_Version_Find
{
   const char *version;
   Gotham_Citizen *citizen;
} Module_Version_Find;

typedef struct _Module_Version_Software
{
   unsigned int *v,
                *p,
                nb_v,
                nb_p;
} Module_Version_Software;

typedef struct _Module_Version
{
   Gotham *gotham;
   Module_Version_Conf *conf;

   Ecore_Timer *poll;
   Eina_Bool sent_once: 1;
} Module_Version;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_version_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_version_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_version_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_version_log_dom, __VA_ARGS__)

void version_alfred_conf_load(Module_Version *version);
void version_alfred_commands_register(void);
void version_alfred_commands_unregister(void);
void version_alfred_command(void *data, Gotham_Citizen_Command *command);
void version_alfred_find_command(void *data, Gotham_Citizen_Command *command);
void version_alfred_answer_get(Module_Version *version, Gotham_Citizen_Command *command);

void version_botman_conf_load(Module_Version *version);
void version_botman_commands_register(void);
void version_botman_commands_unregister(void);
void version_botman_command(void *data, Gotham_Citizen_Command *command);
void version_botman_fetch(void *data, Gotham_Citizen_Command *command, Eina_Bool *update);
Eina_Bool version_botman_poll(void *data);

char * dupf(const char *s, ...);
#endif

