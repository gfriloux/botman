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
 * Authorization macro
 */
#define _AUTH(_a, _b, _c)                                                      \
   {                                                                           \
      DBG("access_allowed[%p] mc[%p]=%s cit[%p]=%s", _a->access_allowed,       \
          _b, _b->command, _c->citizen, _c->citizen->jid);                     \
      if ((_a->access_allowed) && (!_a->access_allowed(_b, _c->citizen)))      \
        {                                                                      \
           gotham_command_send(_c, "Access denied");                           \
           return EINA_TRUE;                                                   \
        }                                                                      \
   }

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module_Module object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Module *mod = data;
   mod->access_allowed = gotham_modules_function_get("access",
                                                     "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * @param data Module_Module object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
#define _CHECKFUNC(c)                                                          \
   if (!strcmp(module_command[1], #c))                                         \
     {                                                                         \
        command->handled = EINA_TRUE;                                          \
        _AUTH(mod, gotham_modules_command_get(".module "#c), command);         \
        func = _module_##c;                                                    \
     }
   /* "debug" */

   Module_Module *mod = data;
   Gotham_Citizen_Command *command = ev;
   const char **module_command = command->command;
   char *(* func)(Module_Module *mod, const char **module_command) = NULL;
   char *answer;

   EINA_SAFETY_ON_NULL_RETURN_VAL(mod, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   DBG("module[%p], command[%p]=%s", mod, command, command->name);

   if (strcmp(command->name, ".module"))
     return EINA_TRUE;

   /**
    * No arguments : list loaded modules
    */
   if (!module_command[1])
     {
        command->handled = EINA_TRUE;
        _AUTH(mod, gotham_modules_command_get(".module"), command);
        answer = _module_list(mod, module_command);
        goto citizen_answer;
     }

   if ((module_command[2]) && !strcmp(module_command[2], "module"))
     {
        command->handled = EINA_TRUE;
        gotham_command_send(command,
                            "Cannot load or unload module “module”.");
        return EINA_TRUE;
     }

   _CHECKFUNC(load);
   _CHECKFUNC(unload);
   _CHECKFUNC(reload);

   if (!func)
     {
        gotham_command_send(command, "Wrong arguments");
        return EINA_TRUE;
     }

   answer = func(mod, module_command);

citizen_answer:
   gotham_citizen_send(command->citizen,
                       (answer) ? answer :
                                  "Unexpected error occured");
   if (answer) free(answer);

   return EINA_TRUE;
#undef _CHECKFUNC
}

#undef _AUTH

/**
 * @}
 */
