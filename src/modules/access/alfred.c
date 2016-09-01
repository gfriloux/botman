#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Gotham.h>
#include "access.h"

/**
 * @addtogroup Gotham_Module_Access
 * @{
 */

/**
 * @brief Register commands that Afred can handle.
 * For each command to register, call @ref gotham_modules_command_add
 */
void
alfred_commands_register(void)
{
   gotham_modules_command_add("access", ".access sync",
                              "[.access sync] - "
                              "This command will synchronize the access "
                              "configuration to all botmans.", NULL);

   gotham_modules_command_add("access", ".access set",
                              "[.access set pattern level] - "
                              "This command will set a new level access "
                              "to a user pattern.", NULL);

   gotham_modules_command_add("access", ".access del",
                              "[.access del pattern] - "
                              "This command will delete the given user "
                              "access.", NULL);

   gotham_modules_command_add("access",
                              ".access add",
                              "[.access add pattern level description] - "
                              "This command will create a new user access "
                              "with the given level 'level' and description.", NULL);

   gotham_modules_command_add("access", ".access",
                              "[.access] - "
                              "This command will list all user accesses.", NULL);
}

/**
 * @brief Unregister Alfred commands.
 * For each command to unregister call @ref gotham_modules_command_del
 */
void
alfred_commands_unregister(void)
{
   gotham_modules_command_del("access", ".access sync");
   gotham_modules_command_del("access", ".access set");
   gotham_modules_command_del("access", ".access del");
   gotham_modules_command_del("access", ".access add");
   gotham_modules_command_del("access", ".access");
}

/**
 * @}
 */
