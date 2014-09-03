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
   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
} Module_Module;


#define CRI(...) EINA_LOG_DOM_CRIT(_module_module_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_module_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_module_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_module_log_dom, __VA_ARGS__)

static inline void
_module_fake_free(void *d EINA_UNUSED, void *d2 EINA_UNUSED)
{}

char * _module_list(Module_Module *mod, const char **module_command);
char * _module_load(Module_Module *mod, const char **module_command);
char * _module_unload(Module_Module *mod, const char **module_command);
char * _module_reload(Module_Module *mod, const char **module_command);

#endif
