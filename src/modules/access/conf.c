#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Gotham.h>
#include <cJSON.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "access.h"

/**
 * @addtogroup Gotham_Module_Access
 * @{
 */

/**
 * @brief Load rules from a cJSON array.
 * This array can be citizen or commands.
 * @param json_array cJSON array
 * @param list Eina_Inlist pointer to fill
 */
void
_access_conf_load_ruleset(cJSON *json_array, Eina_List **list)
{
   int size,
       i;

   if (!json_array) return;

   size = cJSON_GetArraySize(json_array);
   for (i=0; i<size; i++)
     {
        cJSON *rule = NULL,
              *rule_pattern = NULL,
              *rule_level = NULL,
              *rule_desc = NULL;
        Module_Access_Rule *arule;

        rule = cJSON_GetArrayItem(json_array, i);
        if (!rule)
          continue;

        rule_pattern = cJSON_GetObjectItem(rule, "pattern");
        rule_level = cJSON_GetObjectItem(rule, "level");
        rule_desc = cJSON_GetObjectItem(rule, "description");
        if (!rule_pattern || !rule_level || !rule_desc)
          continue;

        DBG("Ruleset %s : %s : %d", json_array->string,
            rule_pattern->valuestring, rule_level->valueint);

        arule = calloc(1, sizeof(Module_Access_Rule));
        arule->pattern = strdup(rule_pattern->valuestring);
        arule->level = rule_level->valueint;
        arule->description = strdup(rule_desc->valuestring);

        *list = eina_list_append(*list, arule);
     }
}

/**
 * @brief Save rules to a cJSON array.
 * @param json_array cJSON array to update
 * @param list Eina_Inlist containing rules
 */
void
_access_conf_save_ruleset(cJSON *json_array, Eina_List *list)
{
   Module_Access_Rule *rule;
   cJSON *json_entry;
   Eina_List *l;

   EINA_LIST_FOREACH(list, l, rule)
     {
        json_entry = cJSON_CreateObject();

        cJSON_AddItemToObject(json_entry, "pattern",
                         cJSON_CreateString(rule->pattern));
        cJSON_AddItemToObject(json_entry, "level",
                         cJSON_CreateNumber((double)rule->level));
        cJSON_AddItemToObject(json_entry, "description",
                         cJSON_CreateString(rule->description));

        cJSON_AddItemReferenceToArray(json_array, json_entry);
     }

}

/**
 * @brief Load conf file into memory.
 * @param access Module_Access structure
 */
void
access_conf_load(Module_Access *access)
{
   cJSON *json,
         *json_ptr;

   DBG("access[%p]", access);

   json = gotham_modules_conf_load(MODULE_ACCESS_CONF);
   if (!json)
     return;

   /* We extract the revision */
   json_ptr = cJSON_GetObjectItem(json, "revision");
   if ((!json_ptr) || (json_ptr->type != cJSON_Number))
     {
        access->revision = -1;
        ERR("Failed to parse configuration file!");
        cJSON_Delete(json);
        return;
     }
   access->revision = json_ptr->valueint;
   DBG("Access revision : %d", access->revision);

   _access_conf_load_ruleset(cJSON_GetObjectItem(json, "citizens"),
                             &access->citizens);
   _access_conf_load_ruleset(cJSON_GetObjectItem(json, "commands"),
                             &access->commands);
   cJSON_Delete(json);
}


/**
 * @brief Save conf in memory into conf file.
 * @param access Module_Access structure
 */
void
access_conf_save(Module_Access *access)
{
   cJSON *json = NULL;

   json = access_conf_tojson(access);
   if (!json)
     return;

   if (!gotham_modules_conf_save(MODULE_ACCESS_CONF, json))
     {
        ERR("An error occured while saving conf file");
     }
   cJSON_Delete(json);
}

/**
 * @brief Turn a Module_Access conf into a JSON structure.
 * @param access Module_Access structure
 * @return cJSON structure containing conf.
 */
cJSON *
access_conf_tojson(Module_Access *access)
{
   cJSON *json = NULL,
         *json_citizens = NULL,
         *json_commands = NULL;

   json = cJSON_CreateObject();
   json_citizens = cJSON_CreateArray();
   json_commands = cJSON_CreateArray();


   if ((!json) || (!json_citizens) || (!json_commands))
     {
        ERR("Failed to create json object");
        cJSON_Delete(json);
        cJSON_Delete(json_citizens);
        cJSON_Delete(json_commands);
        return NULL;
     }

   cJSON_AddItemToObject(json, "revision",
                         cJSON_CreateNumber((double)access->revision));
   cJSON_AddItemToObject(json, "citizens", json_citizens);
   cJSON_AddItemToObject(json, "commands", json_commands);

   _access_conf_save_ruleset(json_citizens, access->citizens);
   _access_conf_save_ruleset(json_commands, access->commands);

   return json;
}

/**
 * @}
 */
