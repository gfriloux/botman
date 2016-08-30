#ifndef SYSINFO_H
#define SYSINFO_H

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define MODULE_CONF SYSCONF_DIR"/gotham/modules.conf.d/sysinfo.conf"

int _module_log_dom;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_log_dom, __VA_ARGS__)

typedef struct _Module_Sysinfo
{
   Gotham *gotham;

   Eina_Hash *hw;       /*!<  Cached informations  */
   Eina_Hash *commands; /*!<  Custom commands, for real-time results */
   Eina_Array *vars;

   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
} Module_Sysinfo;

void conf_load(Module_Sysinfo *obj);

void botman_commands_add(Module_Sysinfo *obj);
void botman_commands_del(Module_Sysinfo *obj);

void alfred_commands_add(Module_Sysinfo *obj);
void alfred_commands_del(Module_Sysinfo *obj);

void alfred_botman_answer_get(Module_Sysinfo *obj, Gotham_Citizen_Command *command);
void alfred_sysinfo_show(Module_Sysinfo *obj, Gotham_Citizen_Command *command);

void botman_sysinfo_get(Module_Sysinfo *obj);
void botman_sysinfo_send(Module_Sysinfo *obj, Gotham_Citizen_Command *command);

void botman_sysinfo_command_run(Module_Sysinfo *obj,
                                Gotham_Citizen_Command *command);
#endif
