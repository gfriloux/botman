#ifndef ASP64_H
#define ASP64_H
#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#include <Escarg.h>

#include "Module_Common_Azy.h"

#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define MODULE_CONF SYSCONF_DIR"/gotham/modules.conf.d/ssh_tunnel.conf"
#define MODULE_SSH_LOG DATA_DIR"/gotham/modules.d/ssh_tunnel/ssh.log"

int _module_log_dom;

typedef struct _Module_Ssh_Tunnel
{
   Gotham *gotham;
   Module_Ssh_Tunnel_Conf *conf;
   const char *conffile;

   struct {
      unsigned int port;
      pid_t pid;
      Ecore_Event_Handler *eh_data,
                          *eh_error,
                          *eh_end;
      Ecore_Exe *exe;
   } tunnel;
   void (*save_conf)(void);
} Module_Ssh_Tunnel;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_log_dom, __VA_ARGS__)

void ssh_tunnel_check(Module_Ssh_Tunnel *obj);
void ssh_tunnel_on(void *data, Gotham_Citizen_Command *command);
void ssh_tunnel_off(void *data, Gotham_Citizen_Command *command);
void ssh_tunnel_get(void *data, Gotham_Citizen_Command *command);

Eina_Bool ssh_tunnel_cb_data(void *data, int type, void *event);
Eina_Bool ssh_tunnel_cb_end(void *data, int type, void *event);

void alfred_command_tunnels_show(void *data, Gotham_Citizen_Command *command);
void botman_answer_get(Module_Ssh_Tunnel *obj,
                       Gotham_Citizen_Command *command);
char * ssh_tunnel_utils_ssh_command(Module_Ssh_Tunnel *obj);
#endif
