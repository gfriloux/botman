#ifndef VERSION_H
#define VERSION_H
#include <Eina.h>
#include <Gotham.h>
#include <time.h>

#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define MODULE_INSTALL_CONF SYSCONF_DIR"/gotham/modules.conf.d/install.conf"

int _module_install_log_dom;

typedef enum _Module_Install_Type
{
   MODULE_INSTALL_TYPE_INSTALL,
   MODULE_INSTALL_TYPE_UPGRADE,
   MODULE_INSTALL_TYPE_JOBS,
   MODULE_INSTALL_TYPE_KILL,
   MODULE_INSTALL_TYPE_WAZUFUK
} Module_Install_Type;


typedef struct _Module_Install
{
   Gotham *gotham;
   Eina_Array *vars;

   Eina_Hash *install,
             *upgrade,
             *jobs;

   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
} Module_Install;

typedef struct _Module_Install_Cmd
{
   Module_Install *install;
   Module_Install_Type type;

   const char **command;
   Gotham_Citizen *citizen;

   Ecore_Event_Handler *evd,
                       *eve,
                       *evr;
   Eina_Strbuf *buf;
   Eina_Bool ok;
} Module_Install_Cmd;

typedef struct _Module_Install_Job
{
   Ecore_Exe *exe;               /*!< Ecore_Exe id                            */
   const char *jid,              /*!< Who requested this action               */
              *cmd;              /*!< Command line and parameters             */
   time_t ts;                    /*!< Timestamp of command launch             */
} Module_Install_Job;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_install_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_install_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_install_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_install_log_dom, __VA_ARGS__)

void module_install_botman_conf_load(Module_Install *install);
void module_install_botman_commands_register(void);
void module_install_botman_commands_unregister(void);

void module_install_event_botman_command(Module_Install *install,
                                         Module_Install_Type type,
                                         Gotham_Citizen_Command *command);

Module_Install_Cmd *module_install_cmd_new(Module_Install *install, Gotham_Citizen_Command *command);
void module_install_cmd_free(Module_Install_Cmd *obj);

void module_install_json_answer(const char *cmd,
                                const char *params,
                                Eina_Bool status,
                                Eina_Strbuf *content,
                                Gotham *gotham,
                                Gotham_Citizen *citizen,
                                Eina_Bool send_to_alfred);


Eina_Bool job_add(Module_Install *install, Ecore_Exe *exe, const char *jid, const char **args);
Eina_Bool job_del(Module_Install *install, Ecore_Exe *exe);
Eina_Bool job_find(Module_Install *install, Ecore_Exe *exe);
Eina_Bool job_kill(Module_Install *install, const char *id);
Eina_List *jobs_list(Module_Install *install);

#endif
