#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "module.h"

#define _ANS(...) { sprintf(answer, __VA_ARGS__); return answer; }

/**
 * @addtogroup Gotham_Module_Module
 * @{
 */

/**
 * @brief List loaded modules in gotham lib.
 * @param mod Module_Module object
 * @param module_command const char ** incoming command
 * @return char * result string
 */
char *
_module_list(Module_Module *mod,
             const char **module_command)
{
   Eina_Strbuf *buf;
   Gotham_Module *module;
   Eina_Bool found = EINA_FALSE;
   char * answer = NULL;

   DBG("mod[%p], module_command[%p]", mod, module_command);

   buf = eina_strbuf_new();
   eina_strbuf_append(buf, "\nLoaded modules :\n");

   EINA_INLIST_FOREACH(gotham_modules_list(), module)
     {
        if (!module)
          continue;

        found = EINA_TRUE;
        eina_strbuf_append_printf(buf,
                                  "\t%s (%s)\n",
                                  module->name,
                                  eina_module_file_get(module->module));
     }

   if (!found)
     eina_strbuf_append(buf, "\tNo modules found.");

   answer = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return answer;
}

/**
 * @brief Load a new module in gotham lib
 * @param mod Module_Module object
 * @param module_command const char ** incoming command
 * @return char * result string
 */
char *
_module_load(Module_Module *mod,
             const char **module_command)
{
   char * answer = NULL;
   const char *name;
   Gotham_Module *gothmod;
   Gotham *gotham = mod->gotham;

   DBG("mod[%p], module_command[%p]", mod, module_command);

   answer = calloc(1, sizeof(char) * 100);

   if (!module_command[2])
     {
        _ANS("Usage : .module load <module_name>");
     }

   name = module_command[2];

   /**
    * Check if module is already loaded
    */
   EINA_INLIST_FOREACH(gotham_modules_list(), gothmod)
     {
        if (strcasecmp(gothmod->name, name))
          continue;
        _ANS("Module %s is already loaded.", name);
     }

   gothmod = gotham_modules_single_module_load(name, gotham);
   if (!gothmod)
     {
        _ANS("Unable to load module %s.", name);
     }

   gotham_modules_register(gotham, gothmod);
   _ANS("Module %s successfully loaded.", name);
}

/**
 * @brief Unload a new module in gotham lib
 * @param mod Module_Module object
 * @param module_command const char ** incoming command
 * @return char * result string
 */
char *
_module_unload(Module_Module *mod,
               const char **module_command)
{
   char * answer = NULL;
   const char *name;
   Gotham_Module *module;
   Eina_Bool found = EINA_FALSE;

   DBG("mod[%p], module_command[%p]", mod, module_command);

   answer = calloc(1, sizeof(char) * 100);

   if (!module_command[2])
     {
        _ANS("Usage : .module unload <module_name>");
     }

   name = module_command[2];

   /**
    * Check if module is loaded
    */
   EINA_INLIST_FOREACH(gotham_modules_list(), module)
     {
        if (strcasecmp(module->name, name))
          continue;
        found = EINA_TRUE;
        break;
     }

   if (!found)
     {
        _ANS("Unable to unload module %s (not loaded)", name);
     }

   gotham_modules_unload(name);
   _ANS("Module %s successfully unloaded.", name);
}

/**
 * @brief Reload a new module in gotham lib
 * @param mod Module_Module object
 * @param module_command const char ** incoming command
 * @return char * result string
 */
char *
_module_reload(Module_Module *mod,
               const char **module_command)
{
   Gotham *gotham;
   char * answer = NULL;
   const char *name;
   Gotham_Module *module;
   Eina_Bool found = EINA_FALSE;

   DBG("mod[%p], module_command[%p]", mod, module_command);

   answer = calloc(1, sizeof(char) * 100);

   if (!module_command[2])
     {
        _ANS("Usage : .module reload <module_name>");
     }

   name = module_command[2];

   /**
    * Check if module is already loaded
    */
   EINA_INLIST_FOREACH(gotham_modules_list(), module)
     {
        if (strcasecmp(module->name, name))
          continue;
        found = EINA_TRUE;
        break;
     }
   if (!found)
     {
        _ANS("Module %s not found", name);
     }

   gotham = mod->gotham;
   gotham_modules_unload(name);

   module = gotham_modules_single_module_load(name, gotham);

   if (!module)
     {
        _ANS("Unable to reload module %s.", name);
     }

   gotham_modules_register(gotham, module);
   _ANS("Module %s successfully reloaded.", name);
}

#undef _ANS

/**
 * @}
 */
