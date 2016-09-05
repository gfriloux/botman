#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "module.h"

/**
 * @addtogroup Gotham_Module_Module
 * @{
 */

/**
 * @brief List loaded modules in gotham lib.
 * @param data Module_Module object
 * @param module_command const char ** incoming command
 */
void
_module_list(void *data,
             Gotham_Citizen_Command *command EINA_UNUSED)
{
   Module_Module *mod = data;
   Eina_Strbuf *buf;
   Gotham_Module *module;

   DBG("mod[%p]", mod);

   buf = eina_strbuf_new();

   EINA_INLIST_FOREACH(gotham_modules_list(), module)
     {
        eina_strbuf_append_printf(buf, "\t%s (%s)\n",
                                  module->name,
                                  eina_module_file_get(module->module));
     }

   if (!eina_strbuf_length_get(buf)) eina_strbuf_append(buf, "\tNo modules found.");
   eina_strbuf_prepend(buf, "\nLoaded modules :\n");
   gotham_citizen_send(command->citizen, eina_strbuf_string_get(buf));
   eina_strbuf_free(buf);
}

/**
 * @brief Load a new module in gotham lib
 * @param data Module_Module object
 * @param module_command const char ** incoming command
 */
void
_module_load(void *data,
             Gotham_Citizen_Command *command)
{
   Module_Module *mod = data;
   const char *name;
   Gotham_Module *gothmod;
   Gotham *gotham = mod->gotham;

   DBG("mod[%p]", mod);

   GOTHAM_IF_SEND_RETURN(!command->command[2], command, "Usage : .module load <module_name>");

   name = command->command[2];

   /**
    * Check if module is already loaded
    */
   EINA_INLIST_FOREACH(gotham_modules_list(), gothmod)
     {
        if (strcasecmp(gothmod->name, name)) continue;
        gotham_citizen_send(command->citizen, "Module is already loaded.");
        return;
     }

   gothmod = gotham_modules_single_module_load(name, gotham);
   GOTHAM_IF_SEND_RETURN(!gothmod, command, "Unable to load module.");

   gotham_modules_register(gotham, gothmod);
   gotham_citizen_send(command->citizen, "Module successfully loaded.");
}

/**
 * @brief Unload a new module in gotham lib
 * @param data Module_Module object
 * @param module_command const char ** incoming command
 */
void
_module_unload(void *data,
               Gotham_Citizen_Command *command)
{
   Module_Module *mod = data;
   const char *name;
   Gotham_Module *module;
   Eina_Bool found = EINA_FALSE;

   DBG("mod[%p]", mod);

   GOTHAM_IF_SEND_RETURN(!command->command[2], command, "Usage : .module unload <module_name>");
   GOTHAM_IF_SEND_RETURN(!strcmp(command->command[2], "module"), command, "Cannot load or unload module “module”.");

   name = command->command[2];

   /**
    * Check if module is loaded
    */
   EINA_INLIST_FOREACH(gotham_modules_list(), module)
     {
        if (strcasecmp(module->name, name)) continue;
        found = EINA_TRUE;
        break;
     }

   GOTHAM_IF_SEND_RETURN(!found, command, "Unable to unload module (not loaded).");

   gotham_modules_unload(name);
   gotham_citizen_send(command->citizen, "Module successfully unloaded.");
}

/**
 * @brief Reload a new module in gotham lib
 * @param mod Module_Module object
 * @param module_command const char ** incoming command
 */
void
_module_reload(void *data,
               Gotham_Citizen_Command *command)
{
   Module_Module *mod = data;
   Gotham *gotham;
   const char *name;
   Gotham_Module *module;
   Eina_Bool found = EINA_FALSE;

   DBG("mod[%p]", mod);

   GOTHAM_IF_SEND_RETURN(!command->command[2], command, "Usage : .module reload <module_name>");
   GOTHAM_IF_SEND_RETURN(!strcmp(command->command[2], "module"), command, "Cannot load or unload module “module”.");

   name = command->command[2];

   /**
    * Check if module is already loaded
    */
   EINA_INLIST_FOREACH(gotham_modules_list(), module)
     {
        if (strcasecmp(module->name, name)) continue;
        found = EINA_TRUE;
        break;
     }

   GOTHAM_IF_SEND_RETURN(!found, command, "Module not found");

   gotham = mod->gotham;
   gotham_modules_unload(name);

   module = gotham_modules_single_module_load(name, gotham);
   GOTHAM_IF_SEND_RETURN(!module, command, "Unable to reload module.");

   gotham_modules_register(gotham, module);
   gotham_citizen_send(command->citizen, "Module successfully reloaded.");
}

#undef _ANS

/**
 * @}
 */
