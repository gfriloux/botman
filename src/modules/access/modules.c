#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Gotham.h>
#include <fnmatch.h>
#include "access.h"

/**
 * @addtogroup Gotham_Module_Access
 * @{
 */

/**
 * @brief Search a specific command's access level.
 * List command rules to find the requested command. If found,
 * return the corresponding access level. Otherwise return 1
 * @param access Module_Access structure
 * @param command Command to search
 * @return Command's access level
 */
unsigned int
modules_commands_level_find(Module_Access *access,
                            const char *command)
{
   Module_Access_Conf_Rule *rule;
   Eina_List *l;

   EINA_LIST_FOREACH(access->conf->commands, l, rule)
     {
        if (!fnmatch(rule->pattern, command, FNM_NOESCAPE))
          return rule->level;
     }

   /* For every command not having a special rule, we set it to 1 */
   return 1;
}

/**
 * @brief Search if module commands matches a specific rule, and set his access levels
 * according to this.
 * If no specific rule is found, command will get default access rights, meaning
 * a level 1 command.
 * @param access Module_Access structure
 * @param citizen Gotham_Module to evaluate
 */
void
modules_commands_check(Module_Access *access,
                       Gotham_Module *module)
{
   Gotham_Module_Command *command;

   EINA_INLIST_FOREACH(module->commands, command)
     {
        unsigned int level;

        level = modules_commands_level_find(access, command->command);
        DBG("command[%p]%s : %d", command, command->command, level);
        gotham_modules_command_var_set(command, "access_level", "%u", level);
     }
}

/**
 * @}
 */
