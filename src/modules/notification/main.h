#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#include "Module_Common_Azy.h"

#define MODULE_NOTIFICATION_CONF SYSCONF_DIR"/gotham/modules.conf.d/notification.conf"

typedef struct _Module_Notification
{
   Gotham *gotham;
   Module_Notification_Conf *conf;
} Module_Notification;

int _module_notification_log_dom;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_notification_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_notification_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_notification_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_notification_log_dom, __VA_ARGS__)

void alfred_group_print(void *data, Gotham_Citizen_Command *command);
void alfred_group_add(void *data, Gotham_Citizen_Command *command);
void alfred_group_del(void *data, Gotham_Citizen_Command *command);
void alfred_user_add(void *data, Gotham_Citizen_Command *command);
void alfred_user_del(void *data, Gotham_Citizen_Command *command);
void alfred_send(void *data, Gotham_Citizen_Command *command);

Module_Notification_Conf_Group * utils_group_find(Module_Notification *notification, const char *name);
const char * utils_user_find(Module_Notification_Conf_Group *group, const char *name);
