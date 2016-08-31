#ifndef SPAM_H
#define SPAM_H
#include <Eina.h>
#include <Gotham.h>

#include "Module_Common_Azy.h"

#define VARGET(_a) gotham_citizen_var_get(citizen, _a)
#define MODULE_SPAM_CONF SYSCONF_DIR"/gotham/modules.conf.d/spam.conf"

int log_dom;

typedef struct _Module_Spam
{
   Gotham *gotham;
   Module_Spam_Conf *conf;
   struct
   {
      Ecore_Timer *t;
      Eina_List *l;
   } queue;
   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
} Module_Spam;

#define CRI(...) EINA_LOG_DOM_CRIT(log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(log_dom, __VA_ARGS__)

void conf_load(Module_Spam *spam);
void queue_add(Module_Spam *spam, Gotham_Citizen *from, Gotham_Citizen *to, const char *command, const char *jid);
void queue_check(Module_Spam *spam, Gotham_Citizen_Command *command);
Eina_Bool queue_timer(void *data);
const char * utils_atos(const char **array, const char *d);
#endif
