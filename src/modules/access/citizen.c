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

#define _WRITE_CONF() gotham_serialize_struct_to_file(access->conf, MODULE_ACCESS_CONF, (Gotham_Serialization_Function)Module_Access_Conf_to_azy_value)

/**
 * @brief Check if given char is a number.
 * @param c char to check
 * @return EINA_TRUE if c is a number, otherwise EINA_FALSE
 */
Eina_Bool _isnumber(char c)
{
   if (c < 48  || c > 57) return EINA_FALSE;
   return EINA_TRUE;
}

/**
 * @brief Search if citizen matches a specific rule, and set his access levels
 * according to this.
 * If no specific rule is found, citizen will get default access rights, meaning
 * a level 1 citizen.
 * @param access Module_Access structure
 * @param citizen Gotham_Citizen to evaluate
 */
void
citizen_access_eval(Module_Access *access, Gotham_Citizen *citizen)
{
   Module_Access_Conf_Rule *rule;
   unsigned int level = 1;
   const char *pattern = "default";
   Eina_List *l;

   EINA_LIST_FOREACH(access->conf->citizens, l, rule)
     {
        if (fnmatch(rule->pattern, citizen->jid, FNM_NOESCAPE))
          continue;

        level = rule->level;
        pattern = rule->pattern;
        break;
     }

   DBG("citizen[%p]%s : %d", citizen, citizen->jid, level);
   gotham_citizen_var_set(citizen, "access_level", "%u", level);
   gotham_citizen_var_set(citizen, "access_pattern", pattern);
}

/**
 * @brief List all citizen and their access levels.
 * @param data Module_Access structure
 * @param command Incoming message (splitted)
 */
void
citizen_access_list(void *data,
                    Gotham_Citizen_Command *command)
{
   Module_Access *access = data;
   Eina_Strbuf *buf;
   unsigned int i = 0;
   Module_Access_Conf_Rule *rule;
   Eina_List *l;

   if (command->command[1]) return;

   buf = eina_strbuf_new();
   eina_strbuf_append(buf, "\nList of access rights :\n");

   EINA_LIST_FOREACH(access->conf->citizens, l, rule)
     {
        eina_strbuf_append_printf(buf,
                                  "\t%d/\tPattern [%s] → Access level [%d]\n"
                                  "\t\t%s\n",
                                  i++, rule->pattern, rule->level,
                                  rule->description);
     }

   gotham_command_send(command, eina_strbuf_string_get(buf));
   eina_strbuf_free(buf);
}

/**
 * @brief Set new access levels to a pattern.
 * @param module_data Module_Access structure
 * @param command Incoming message (splitted)
 */
void
citizen_access_set(void *module_data,
                   Gotham_Citizen_Command *command)
{
   Module_Access *access = module_data;
   Eina_Iterator *it;
   const char *pattern;
   unsigned int level;
   Module_Access_Conf_Rule *rule,
                           *rule_found = NULL;
   void *data;
   const char **cmd = command->command;
   Eina_List *l;

   GOTHAM_IF_SEND_RETURN((!cmd[2]) || (!cmd[3]) || (!_isnumber(cmd[3][0])), command, "Wrong use of .access set");

   pattern = cmd[2];
   level = atoi(cmd[3]);

   EINA_LIST_FOREACH(access->conf->citizens, l, rule)
     {
        DBG("rule->pattern=%s pattern=%s", rule->pattern, pattern);

        if (strcasecmp(rule->pattern, pattern))
          continue;

        rule->level = level;
        rule_found = rule;
     }

   GOTHAM_IF_SEND_RETURN(!rule_found, command, "You gave me an unknown pattern!");

   it = eina_hash_iterator_data_new(access->gotham->citizens);
   while (eina_iterator_next(it, &data))
     {
        Gotham_Citizen *citizen = data;
        const char *citizen_pattern;

        citizen_pattern = gotham_citizen_var_get(citizen, "access_pattern");
        if (!citizen_pattern) continue;

        if (strcasecmp(citizen_pattern, rule_found->pattern)) continue;

        gotham_citizen_var_set(citizen, "access_level", "%u",
                               rule_found->level);
     }

   /* We should sync to disk */
   access->conf->revision++;
   _WRITE_CONF();
   gotham_command_send(command, "Modification done");
}

/**
 * @brief Remove a specific user access pattern.
 * @param module_data Module_Access
 * @param command Incoming message (splitted)
 */
void
citizen_access_del(void *module_data,
                   Gotham_Citizen_Command *command)
{
   Module_Access *access = module_data;
   Eina_Iterator *it;
   void *data;
   const char **cmd = command->command;
   const char *pattern = cmd[2];
   Module_Access_Conf_Rule *rule;
   Eina_List *l,
             *l2;
   Eina_Bool found = EINA_FALSE;

   DBG("access[%p] command[%p]", access, command);

   GOTHAM_IF_SEND_RETURN(!pattern, command, "Wrong use of .access del");
   EINA_LIST_FOREACH_SAFE(access->conf->citizens, l, l2, rule)
     {
        if (strcasecmp(pattern, rule->pattern)) continue;

        access->conf->citizens = eina_list_remove(access->conf->citizens, rule);
        free((char *)rule->pattern);
        free((char *)rule->description);
        free(rule);
        found = EINA_TRUE;
        break;
     }

   GOTHAM_IF_SEND_RETURN(!found, command, "Pattern not found");

   it = eina_hash_iterator_data_new(access->gotham->citizens);
   while (eina_iterator_next(it, &data))
     {
        Gotham_Citizen *citizen = data;
        const char *citizen_pattern;

        citizen_pattern = gotham_citizen_var_get(citizen, "access_pattern");
        if (!citizen_pattern) continue;
        if (strcasecmp(pattern, citizen_pattern)) continue;

        citizen_access_eval(access, citizen);
     }

   access->conf->revision++;
   _WRITE_CONF();
   gotham_command_send(command, "Modification done");
}

/**
 * @brief Add a new access pattern.
 * @param module_data Module_Access structure
 * @param command Incoming message (splitted)
 */
void
citizen_access_add(void *module_data,
                   Gotham_Citizen_Command *command)
{
   Module_Access *access = module_data;
   Module_Access_Conf_Rule *rule;
   Eina_Iterator *it;
   void *data;
   const char **cmd = command->command;
   const char *p;

   GOTHAM_IF_SEND_RETURN((!cmd[2])               ||
                         (!cmd[3])               ||
                         (!_isnumber(cmd[3][0])) ||
                         (!cmd[4]),
                         command, "Modification done");

   rule = calloc(1, sizeof(Module_Access_Conf_Rule));
   rule->pattern = strdup(cmd[2]);
   rule->level = atoi(cmd[3]);

   /* take all parameters of message from [4] to the end */
   p = command->message;
   p += (strlen(cmd[0]) +
         strlen(cmd[1]) +
         strlen(cmd[2]) +
         strlen(cmd[3]) +
         4);
   rule->description = strdup(p);

   DBG("description : %s", rule->description);

   access->conf->citizens = eina_list_append(access->conf->citizens, rule);

   it = eina_hash_iterator_data_new(access->gotham->citizens);
   while (eina_iterator_next(it, &data))
     {
        Gotham_Citizen *citizen = data;

        if (fnmatch(rule->pattern, citizen->jid, FNM_NOESCAPE))
          continue;

        DBG("Citizen %s matches %s", citizen->jid, rule->pattern);
        gotham_citizen_var_set(citizen, "access_level", "%u", rule->level);
        gotham_citizen_var_set(citizen, "access_pattern", rule->pattern);
     }

   access->conf->revision++;
   _WRITE_CONF();
   gotham_command_send(command, "Modification done");
}

/**
 * @brief Sync access levels to botmans.
 * Load access rules in a cJSON structure,
 * run through citizens and send access rules if citizen is a Botman.
 * @param module_data Module_Access
 * @param command Incoming message (splitted)
 */
void
citizen_access_sync(void *module_data,
                    Gotham_Citizen_Command *command)
{
   Module_Access *access = module_data;
   char *s;
   Eina_Strbuf *buf;
   Eina_Iterator *it;
   void *data;

   s = gotham_serialize_struct_to_string(access, (Gotham_Serialization_Function)Module_Access_Conf_to_azy_value);

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, ".access set %s", s);
   free(s);

   it = eina_hash_iterator_data_new(access->gotham->citizens);
   while(eina_iterator_next(it, &data))
     {
        Gotham_Citizen *citizen = data;
        if (citizen->type != GOTHAM_CITIZEN_TYPE_BOTMAN) continue;

        gotham_citizen_send(citizen, eina_strbuf_string_get(buf));
     }
   eina_strbuf_free(buf);

   gotham_command_send(command, "Everyone seems to be up2date");
}

#undef _WRITE_CONF

/**
 * @}
 */
