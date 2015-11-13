#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE
#include <ctype.h>

#include "gotham_private.h"

/**
 * @addtogroup Libgotham_Modules_Functions
 * @{
 */

/**
 * Internal list of loaded modules in this Gotham instance
 */
static Eina_Inlist *_gotham_modules_list = NULL;

/**
 * Callback for freeing a module command
 * @param data Data to free
 */
static void
_gotham_modules_command_var_free(void *data)
{
   free((char *)data);
}

/**
 * @return the global _gotham_modules_list
 */
Eina_Inlist *
gotham_modules_list(void)
{
   return _gotham_modules_list;
}

/**
 * Add (register) a command of a module. A command has to be registerd
 * otherwise we won't be able to use it.
 * @param module_name Module to check
 * @param command Command name to register
 * @param desc Command short description
 * @return EINA_TRUE if command was added successfully,
 *          EINA_FALSE otherwise
 */
Eina_Bool
gotham_modules_command_add(const char *module_name,
                           const char *command,
                           const char *desc)
{
   Gotham_Module *module;
   Gotham_Module_Command *mc;

   DBG("module_name[%p]%s command[%p]%s",
       module_name, module_name, command, command);

   EINA_INLIST_FOREACH(_gotham_modules_list, module)
     {
        if (strcasecmp(module->name, module_name))
          continue;

        mc = calloc(1, sizeof(Gotham_Module_Command));
        mc->command = strdup(command);
        mc->desc = strdup(desc);
        mc->vars = eina_hash_string_superfast_new(_gotham_modules_command_var_free);

        module->commands = eina_inlist_append(module->commands,
                                              EINA_INLIST_GET(mc));
        return EINA_TRUE;
     }

   return EINA_FALSE;
}

/**
 * Remove a registered command from a module
 * @param module_name Module name to check
 * @param command Command to remove
 */
void
gotham_modules_command_del(const char *module_name,
                           const char *command)
{
   Eina_Inlist *l;
   Gotham_Module *m;
   Gotham_Module_Command *mc;

   DBG("module_name[%p]%s command[%p]%s",
       module_name, module_name, command, command);

   EINA_INLIST_FOREACH(_gotham_modules_list, m)
     {
        if (strcasecmp(m->name, module_name))
          continue;

        EINA_INLIST_FOREACH_SAFE(m->commands, l, mc)
          {
             if (strcasecmp(mc->command, command))
               continue;

             m->commands = eina_inlist_remove(m->commands, EINA_INLIST_GET(mc));
             return;
          }
     }

   return;
}

/**
 * Checks if a module should be loaded. The module MUST have some functions :
 * module_init, module_register, module_unregister, module_shutdown; otherwise
 * it won't be loaded.
 * The module also has to be present in conf->modules list of Gotham structure
 * @param module Eina_Module to check
 * @param data Gotham structure
 * @return EINA_TRUE if module was loaded, EINA_FALSE otherwise
 */
static Eina_Bool
_gotham_module_check(Eina_Module *module, void *data)
{
#define _ERR(...)                                                              \
   {                                                                           \
      ERR(__VA_ARGS__);                                                        \
      eina_module_unload(module);                                              \
      if (gothmod)                                                             \
        free(gothmod);                                                         \
      return EINA_FALSE;                                                       \
   }
   Gotham_Module *gothmod = NULL;
   Gotham_Module_Init module_init;
   Gotham_Module_Register module_register;
   Gotham_Module_Unregister module_unregister;
   Gotham_Module_Shutdown module_shutdown;
   const char *name;
   char *modname;
   Eina_List *l;
   Eina_Bool found = EINA_FALSE;
   Gotham *gotham = data;

   if (!eina_module_load(module))
     {
        ERR("Could not load module '%s'. Error : '%s'",
            eina_module_file_get(module), eina_error_msg_get(eina_error_get()));
        return EINA_FALSE;
     }

#define EMSG(name) module_##name = eina_module_symbol_get(module, "module_"#name)
   EMSG(init);
   EMSG(register);
   EMSG(unregister);
   EMSG(shutdown);
#undef EMSG

   if ((!module_init) || (!module_register) ||
       (!module_unregister) || (!module_shutdown))
     {
        _ERR("Failed to load all the functions for module %s",
             eina_module_file_get(module));
     }

   name = module_init();
   if (!name)
     {
        _ERR("Init function from module '%s' failed. Unloading",
             eina_module_file_get(module));
     }

   gothmod = calloc(1, sizeof(Gotham_Module));
   if (!gothmod)
     {
        _ERR("Failed to allocate Gotham_Module for %s",
             eina_module_file_get(module));
     }

   gothmod->module_init = module_init;
   gothmod->module_register = module_register;
   gothmod->module_unregister = module_unregister;
   gothmod->module_shutdown = module_shutdown;
   gothmod->module = module;
   gothmod->name = name;

   EINA_LIST_FOREACH(gotham->conf->modules, l, modname)
     {
        if (strcasecmp(modname, name)) continue;
        found = EINA_TRUE;
        break;
     }

   if (!found)
     {
        NFO("Module %s should not be loaded", gothmod->name);
        eina_module_unload(module);
        free(gothmod);
        return EINA_FALSE;
     }

   _gotham_modules_list = eina_inlist_append(_gotham_modules_list,
                                             EINA_INLIST_GET(gothmod));

   return EINA_TRUE;
#undef _ERR
}

/**
 * List modules located in GOTHAM_MODULE_PATH, and load them if they have
 * to using _gotham_module_check
 * @param gotham Gotham structure
 */
void
_gotham_modules_load(Gotham *gotham)
{
   Eina_Array *mods;
   DBG("Looking for modules in %s", GOTHAM_MODULE_PATH);

   mods = eina_module_list_get(NULL, GOTHAM_MODULE_PATH, EINA_FALSE,
                               _gotham_module_check, gotham);
   eina_array_free(mods);
   DBG("Loaded %d modules", eina_inlist_count(_gotham_modules_list));
}

/**
 * Load a single module (if it meets requirements found in
 * _gotham_module_check)
 * @param name Module name to load (without .so extension)
 * @param gotham Gotham structure
 * @return Gotham_Module structure representing loaded module
 */
Gotham_Module *
gotham_modules_single_module_load(const char *name, Gotham *gotham)
{
   Eina_Bool ret = EINA_FALSE;
   Eina_Iterator *f_it;
   const Eina_File_Direct_Info *f_info;
   char *name_so;
   Eina_Module *module = NULL;
   Eina_Inlist *l;
   Gotham_Module *m = NULL,
                 *gothmod = NULL;

   name_so = calloc(1, strlen(name)+3);
   sprintf(name_so, "%s.so", name);
   f_it = eina_file_direct_ls(GOTHAM_MODULE_PATH);
   EINA_ITERATOR_FOREACH(f_it, f_info)
     {
        const char *ptr = &(f_info->path[f_info->name_start]);
        if (strcasecmp(ptr, name_so))
          continue;

        module = eina_module_new(f_info->path);
        ret = _gotham_module_check(module, gotham);
        if (!ret)
          {
             DBG("Unable to load module file %s.", f_info->path);
             break;
          }
        DBG("Module file %s successfully loaded.", f_info->path);
        break;
     }
   eina_iterator_free(f_it);
   free(name_so);
   if ((!module) || (!ret))
     return NULL;

   /**
    * Now we retrieve Gotham_Module object and send it back to caller
    */
   EINA_INLIST_FOREACH_SAFE(_gotham_modules_list, l, m)
     {
        if (m->module != module)
          continue;
        gothmod = m;
        break;
     }

   /**
    * Call ecore event to set access rights on module
    */
   ecore_event_add(GOTHAM_EVENT_MODULES_SINGLE_READY, gothmod,
                   _gotham_fake_free, NULL);

   return gothmod;
}

/**
 * Unload the module matching name by calling unregister and shutdown,
 * and then removing it from the global _gotham_modules_list
 * @param name Module name to unload
 */
void
gotham_modules_unload(const char *name)
{
   Eina_Inlist *l;
   Gotham_Module *m = NULL;
   Ecore_Event_Handler *ev;

   EINA_INLIST_FOREACH_SAFE(_gotham_modules_list, l, m)
     {
        if (strcasecmp(m->name, name))
          continue;

        _gotham_modules_list = eina_inlist_remove(_gotham_modules_list,
                                                  EINA_INLIST_GET(m));
        EINA_LIST_FREE(m->evs, ev)
          ecore_event_handler_del(ev);

        m->module_unregister(m->module_data);
        m->module_shutdown();
        eina_module_unload(m->module);

        free(m);
        return;
     }
}

/**
 * Add or update a module command variable in his hash list
 * @param command Gotham_Module_Command
 * @param name Variable name
 * @param value Value to set. If value is a format, __VA_ARGS__
 *              must be provided
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_modules_command_var_set(Gotham_Module_Command *command,
                               const char *name,
                               const char *value,
                               ...)
{
   va_list args;
   char *ptr,
        *str;
   size_t len;

   va_start(args, value);
   len = vasprintf(&str, value, args);
   va_end(args);

   DBG("name=%s value=%s str=%s", name, value, str);

   if (len <=  0 || !str)
     return EINA_FALSE;

   ptr = (char *)eina_hash_modify(command->vars, name, str);
   if (!ptr)
     return eina_hash_add(command->vars, name, str);

   free((char *)ptr);
   return EINA_TRUE;
}

/**
 * Get a module_command variable
 * @param command Gotham_Module_Command
 * @param name Variable name to search for
 * @return Value matching given name, or null if not found
 */
const char *
gotham_modules_command_var_get(Gotham_Module_Command *command,
                               const char *name)
{
   return (const char *)eina_hash_find(command->vars, name);
}

/**
 * Get a module command matching the given command name
 * @param cmd Command to search
 * @return Gotham_Module_Command structure if found, NULL otherwise
 */
Gotham_Module_Command *
gotham_modules_command_get(const char *cmd)
{
   Eina_Inlist *l;
   Gotham_Module *m;
   Gotham_Module_Command *mc;

   DBG("cmd[%p]=“%s”", cmd, cmd);

   EINA_INLIST_FOREACH(_gotham_modules_list, m)
     {
        EINA_INLIST_FOREACH_SAFE(m->commands, l, mc)
          {
             if (strcasecmp(mc->command, cmd))
               continue;

             DBG("Found !");
             return mc;
          }
     }

   return NULL;
}

/**
 * Get the pointer of a module function
 * @param module Module name
 * @param function Function name
 * @return Function pointer if found, NULL otherwise
 */
void *
gotham_modules_function_get(const char *module, const char *function)
{
   Gotham_Module *m;

   EINA_INLIST_FOREACH(_gotham_modules_list, m)
     {
        if (strcasecmp(m->name, module))
          continue;
        return eina_module_symbol_get(m->module, function);
     }
   return NULL;
}

/**
 * Register a module to the Gotham structure, by registering
 * every GOTHAM_EVENT the module could need
 * @param gotham Gotham structure
 * @param module Gotham_Module to register
 */
void
gotham_modules_register(Gotham *gotham, Gotham_Module *module)
{
   Gotham_Module_Event module_event;

   module->module_data = module->module_register(gotham);
   if (!module->module_data)
     return;

#define EVLOAD(name, event)                                                    \
   {                                                                           \
      module_event = eina_module_symbol_get(module->module, "event_"#name);    \
      if (module_event)                                                        \
        {                                                                      \
           Ecore_Event_Handler *ev;                                            \
           ev = ecore_event_handler_add(GOTHAM_EVENT_##event,                  \
                                        module_event,                          \
                                        module->module_data);                  \
           module->evs = eina_list_append(module->evs, ev);                    \
        }                                                                      \
   }

   EVLOAD(connect, CONNECT);
   EVLOAD(disconnect, DISCONNECT);
   EVLOAD(modules_single_ready, MODULES_SINGLE_READY);
   EVLOAD(modules_ready, MODULES_READY);
   EVLOAD(citizen_connect, CITIZEN_CONNECT);
   EVLOAD(citizen_disconnect, CITIZEN_DISCONNECT);
   EVLOAD(citizen_command, CITIZEN_COMMAND);
   EVLOAD(citizen_list, CITIZEN_LIST);
   EVLOAD(citizen_info, CITIZEN_INFO);
   EVLOAD(citizen_idle, CITIZEN_IDLE);
   EVLOAD(citizen_new, CITIZEN_NEW);
   EVLOAD(citizen_leave, CITIZEN_LEAVE);
#undef EVLOAD
}

/**
 * @}
 */
