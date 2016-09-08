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
 * @brief Register commands that a Botman can handle.
 * Command list :<br />
 * .access : will give the revision number of access configuration
 * For each command to register, call @ref gotham_modules_command_add
 */
void
botman_commands_register(void)
{
   gotham_modules_command_add("access", ".access",
                              "This command will list all user accesses.",
                              citizen_access_list);
   gotham_modules_command_add("access", ".access set",
                              "This command will set access rights",
                              botman_access_sync);
}

/**
 * @brief Unregister Botman's commands.
 * For each command to unregister call @ref gotham_modules_command_del
 */
void
botman_commands_unregister(void)
{
   gotham_modules_command_del("access", ".access");
   gotham_modules_command_del("access", ".access set");
}

/**
 * @brief For a Botman, add Alfred as a level 10 user.
 * Alfred should be allowed to send anything to a botman. We use alfred's account
 * defined as a Gotham_Citizen in access->gotham->alfred
 * @param access Module_Access structure
 */
void
botman_access_alfred_add(Module_Access *access)
{
   Gotham_Citizen *alfred = access->gotham->alfred;
   Module_Access_Conf_Rule *arule;

   DBG("access[%p]", access);

   arule = Module_Access_Conf_Rule_new();
   eina_stringshare_replace(&arule->pattern, alfred->jid);
   eina_stringshare_replace(&arule->description, alfred->nickname ? alfred->nickname : "Alfred Pennyworth");
   arule->level = 10;
   access->conf->citizens = eina_list_append(access->conf->citizens, arule);

   return;
}

/**
 * @brief Set new access rules to a botman from an incoming message.
 * @param data Module_Access structure
 * @param command Gotham_Citizen_Command containing new access rules
 */
void
botman_access_sync(void *data,
                   Gotham_Citizen_Command *command)
{
   Module_Access *access = data;
   Module_Access_Conf *conf;
   const char *p;

   EINA_SAFETY_ON_NULL_RETURN(access);
   EINA_SAFETY_ON_NULL_RETURN(command);

   DBG("access[%p]", access);

   /**
    * Access definition starts on command->commmand[2].
    * So we can offset command->message' start and use it
    */
   p = command->message;
   p += (strlen(command->command[0]) + strlen(command->command[1]) + 2);
   DBG("Offsetted buffer :\n%s", p);

   conf = gotham_serialize_string_to_struct(p, strlen(p), (Gotham_Deserialization_Function)azy_value_to_Module_Access_Conf);
   EINA_SAFETY_ON_NULL_RETURN(conf);

   Module_Access_Conf_free(access->conf);
   access->conf = conf;

   gotham_serialize_struct_to_file(access->conf, MODULE_ACCESS_CONF, (Gotham_Serialization_Function)Module_Access_Conf_to_azy_value);
   botman_access_alfred_add(access);
}

/**
 * @}
 */
