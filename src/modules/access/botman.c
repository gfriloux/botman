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
                              "This command will list all user accesses.");
}

/**
 * @brief Unregister Botman's commands.
 * For each command to unregister call @ref gotham_modules_command_del
 */
void
botman_commands_unregister(void)
{
   gotham_modules_command_del("access", ".access");
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
   Module_Access_Rule *arule;

   DBG("access[%p]", access);

   arule = calloc(1, sizeof(Module_Access_Rule));
   arule->pattern = strdup(alfred->jid);
   arule->level = 10;
   arule->description = strdup(alfred->nickname ?
                                 alfred->nickname :
                                 "Alfred Pennyworth");

   access->citizens = eina_inlist_append(access->citizens, EINA_INLIST_GET(arule));

   return;
}

/**
 * @brief Set new access rules to a botman from an incoming message.
 * @param access Module_Access structure
 * @param command Gotham_Citizen_Command containing new access rules
 */
void
botman_access_sync(Module_Access *access, Gotham_Citizen_Command *command)
{
   cJSON *json;
   Module_Access_Rule *rule;
   Eina_Inlist *l;
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

   json = cJSON_Parse(p);
   if (!json)
     {
        ERR("Invalid access conf from %s", command->citizen->jid);
        return;
     }

   if (!gotham_modules_conf_save(MODULE_ACCESS_CONF, json))
     {
        ERR("An error occured while saving conf file");
        cJSON_Delete(json);
        return;
     }

   EINA_INLIST_FOREACH_SAFE(access->citizens, l, rule)
     {
        access->citizens = eina_inlist_remove(access->citizens,
                                              EINA_INLIST_GET(rule));
        free((char *)rule->pattern);
        free((char *)rule->description);
        free(rule);
     }

   EINA_INLIST_FOREACH_SAFE(access->commands, l, rule)
     {
        access->commands = eina_inlist_remove(access->commands,
                                              EINA_INLIST_GET(rule));
        free((char *)rule->pattern);
        free((char *)rule->description);
        free(rule);
     }

   botman_access_alfred_add(access);
   access_conf_load(access);
}

/**
 * @}
 */
