#ifndef ALERT_H
#define ALERT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#include "Module_Common_Azy.h"

#define MODULE_ALERT_CONF SYSCONF_DIR"/gotham/modules.conf.d/alert.conf"

int _module_alert_log_dom;

typedef struct _Module_Alert
{
   Gotham *gotham;

   Module_Alert_Conf *conf;
   Eina_List *commands;
} Module_Alert;

typedef struct _Module_Alert_Command
{
   Gotham *gotham;

   Module_Alert_Conf_Command *command;

   Ecore_Timer *timer;
   Ecore_Exe *exe;

   Eina_Strbuf *buf;

   struct
   {
      Ecore_Event_Handler *data,
                          *del,
                          *error;
   } ev;

} Module_Alert_Command;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_alert_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_alert_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_alert_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_alert_log_dom, __VA_ARGS__)

Module_Alert_Command * alert_command_new(Gotham *gotham, Module_Alert_Conf_Command *command);
#endif
