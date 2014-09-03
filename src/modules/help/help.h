#ifndef HELP_H
#define HELP_H

#include <Eina.h>
#include <Gotham.h>

int _module_help_log_dom;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_help_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_help_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_help_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_help_log_dom, __VA_ARGS__)

typedef struct _Module_Help
{
   Gotham *gotham;
   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
} Module_Help;

#endif
