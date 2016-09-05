#ifndef MODULE_H
#define MODULE_H
#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#define VARSET(_a, _b, ...) gotham_citizen_var_set(citizen, _a, _b, __VA_ARGS__)
#define VARGET(_a) gotham_citizen_var_get(citizen, _a)

int _module_module_log_dom;

typedef struct _Module_Module
{
   Gotham *gotham;
} Module_Module;


#define CRI(...) EINA_LOG_DOM_CRIT(_module_module_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_module_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_module_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_module_log_dom, __VA_ARGS__)

static inline void
_module_fake_free(void *d EINA_UNUSED, void *d2 EINA_UNUSED)
{}

void _module_list(void *data, Gotham_Citizen_Command *command);
void _module_load(void *data, Gotham_Citizen_Command *command);
void _module_unload(void *data, Gotham_Citizen_Command *command);
void _module_reload(void *data, Gotham_Citizen_Command *command);

#endif
