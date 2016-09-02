#ifndef ASP64_H
#define ASP64_H
#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define MODULE_CONF SYSCONF_DIR"/gotham/modules.conf.d/ssh_tunnel.conf"

int _module_log_dom;

typedef struct _Module_Ssh_Tunnel
{
   Gotham *gotham;

   struct
   {
      const char *host,
                 *login,
                 *key;
      unsigned int port;
   } infos;

   struct {
      unsigned int port;
      pid_t pid;
      Ecore_Event_Handler *eh_data,
                          *eh_end;
      Ecore_Exe *exe;
   } tunnel;

   Eina_List *vars;

   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
   void (*save_conf)(void);
} Module_Ssh_Tunnel;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_log_dom, __VA_ARGS__)

#define AUTH(_a, _b, _c)                                                       \
{                                                                              \
   if ((_a->access_allowed) && (!_a->access_allowed(_b, _c)))                  \
     {                                                                         \
        Eina_Strbuf *buf = eina_strbuf_new();                                  \
        ERR("%s is not autorized", _c->jid);                                   \
        eina_strbuf_append(buf, "Access denied");                              \
        gotham_command_json_answer(".ssh", "", EINA_FALSE,                     \
                                   buf, _a->gotham, _c, EINA_FALSE);           \
        eina_strbuf_free(buf);                                                 \
        return;                                                                \
     }                                                                         \
}
/* "debug" */

void conf_load(Module_Ssh_Tunnel *obj);

void ssh_tunnel_check(Module_Ssh_Tunnel *obj);
void ssh_tunnel_on(void *data, Gotham_Citizen_Command *command);
void ssh_tunnel_off(void *data, Gotham_Citizen_Command *command);
void ssh_tunnel_get(void *data, Gotham_Citizen_Command *command);

Eina_Bool ssh_tunnel_cb_data(void *data, int type, void *event);
Eina_Bool ssh_tunnel_cb_end(void *data, int type, void *event);

void alfred_command_tunnels_show(void *data, Gotham_Citizen_Command *command);
void botman_answer_get(Module_Ssh_Tunnel *obj,
                       Gotham_Citizen_Command *command);
#endif
