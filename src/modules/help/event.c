#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Gotham.h>

#include "help.h"

/**
 * @addtogroup Gotham_Module_Help
 * @{
 */

/**
 * Authorization macro
 */
#define AUTH(_a, _b, _c)                                                       \
   {                                                                           \
      if ((_a) && (!_a(_b, _c->citizen)))                                      \
        {                                                                      \
           gotham_command_send(_c, "Access denied");                           \
           return EINA_TRUE;                                                   \
        }                                                                      \
   }
/* "debug" */

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module_Help structure
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Help *help = data;
   help->access_allowed = gotham_modules_function_get("access",
                                                      "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * Run through gotham_modules_list, get registered commands for each module.
 * Send back the functions list with description
 * @param data Module_Help structure
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Help *help = data;
   Gotham_Citizen_Command *command = ev;
   Gotham_Module *module;
   Eina_Strbuf *buf;

   EINA_SAFETY_ON_NULL_RETURN_VAL(help, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   DBG("help[%p] command[%p]=%s", help, command, command->name);

   if (strcmp(command->name, ".help"))
     return EINA_TRUE;

   if ((command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN) ||
       (command->citizen->type == GOTHAM_CITIZEN_TYPE_ALFRED))
     return EINA_TRUE;

   command->handled = EINA_TRUE;

   AUTH(help->access_allowed, gotham_modules_command_get(".help"),
        command);

   buf = eina_strbuf_new();
   eina_strbuf_append(buf, "\nList of commands :\n");

   EINA_INLIST_FOREACH(gotham_modules_list(), module)
     {
        Eina_Bool found = EINA_FALSE;
        Gotham_Module_Command *mc;
        Eina_Strbuf *mod_buf = eina_strbuf_new();

        EINA_INLIST_FOREACH(module->commands, mc)
          {
             const char *tmp;
             int cmd_lvl;

             if ((help->access_allowed) &&
                 (!help->access_allowed(mc, command->citizen)))
               continue;

             tmp = gotham_modules_command_var_get(mc, "access_level");
             cmd_lvl = tmp ? atoi(tmp) : 1;

             eina_strbuf_append_printf(mod_buf, "\t\t%s (%d) : %s\n",
                                       mc->command,
                                       cmd_lvl,
                                       mc->desc ? mc->desc : "");
             found = EINA_TRUE;
          }

        if (!found)
          {
             eina_strbuf_free(mod_buf);
             continue;
          }

        eina_strbuf_append_printf(buf, "\tCommands for module %s :\n",
                                  module->name);
        eina_strbuf_append(buf, eina_strbuf_string_get(mod_buf));
        eina_strbuf_append(buf, "\n");

        eina_strbuf_free(mod_buf);
     }

   gotham_command_send(command, eina_strbuf_string_get(buf));
   eina_strbuf_free(buf);
   return EINA_TRUE;
}

#undef AUTH

/**
 * @}
 */
