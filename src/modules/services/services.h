#ifndef SERVICES_H
# define SERVICES_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Eina.h>
# include <Ecore.h>
# include <Gotham.h>

# include "Module_Common_Azy.h"

#define MODULE_SERVICES_CONF SYSCONF_DIR"/gotham/modules.conf.d/services.conf"

int _module_services_log_dom;

typedef struct _Module_Services
{
   Gotham *gotham;
   Module_Services_Conf *conf;
} Module_Services;


#define CRI(...) EINA_LOG_DOM_CRIT(_module_services_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_services_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_services_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_services_log_dom, __VA_ARGS__)

void command_start(Module_Services *services, Gotham_Citizen_Command *command);
void command_restart(Module_Services *services, Gotham_Citizen_Command *command);
void command_stop(Module_Services *services, Gotham_Citizen_Command *command);
void command_status(Module_Services *services, Gotham_Citizen_Command *command);

#ifdef _WIN32
Eina_Bool command_win32_start(Module_Services *services, Gotham_Citizen_Command *command, const char *name);
Eina_Bool command_win32_stop(Module_Services *services, Gotham_Citizen_Command *command, const char *name);
Eina_Bool command_win32_status(Module_Services *services, Gotham_Citizen_Command *command, const char *name);
#endif

void module_json_answer(const char *cmd, const char *params, Eina_Bool status, const char *content, Gotham *gotham, const char *jid);

char * utils_strdupf(const char *s, ...);
#endif
