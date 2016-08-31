#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cJSON.h>
#include "version.h"

/**
 * @addtogroup Gotham_Module_Version
 * @{
 */

/**
 * @brief Load custom vars.
 * @param version Module object
 * @param json cJSON object to read
 */
static void
_version_conf_pattern_vars_load(Module_Version *version, cJSON *json)
{
   cJSON *json_obj;
   int i,
       vars_count;

   json_obj = cJSON_GetObjectItem(json, "vars");
   if (!json_obj)
     return;

   vars_count = cJSON_GetArraySize(json_obj);

   for (i=0; i < vars_count; i++)
     {
        cJSON *var_name = cJSON_GetArrayItem(json_obj, i);
        NFO("Found var %s", var_name->valuestring);
        version->vars = eina_list_append(version->vars, strdup(var_name->valuestring));
     }
}

/**
 * @brief Get the default system call to run for getting version information.
 * @param version Module object
 * @param json cJSON object to read.
 */
static void
_version_conf_default_command_load(Module_Version *version, cJSON *json)
{
   cJSON *json_obj;

   json_obj = cJSON_GetObjectItem(json, "default_cmd");
   if (!json_obj)
     return;

   version->versions.default_cmd = eina_stringshare_add(json_obj->valuestring);
   NFO("Found default command “%s”", version->versions.default_cmd);
}

/**
 * @brief Load the software list.
 * Run through json array to get softwares. If element is a single string,
 * simply store it. If it's an object, we have to go inside and get its
 * specific system call.
 * @param version Module object
 * @param json cJSON object to read
 */
static void
_version_conf_versions_list_load(Module_Version *version, cJSON *json)
{
   cJSON *json_obj,
         *json_cur;
   Module_Version_Element *e;
   Eina_List *l = NULL;

   json_obj = cJSON_GetObjectItem(json, "versions_list");
   if (!json_obj)
     return;

   json_cur = cJSON_GetArrayItem(json_obj, 0);
   if (!json_cur)
     return;

   while (1)
     {
        if (!json_cur) break;

        if (json_cur->type == cJSON_String)
          {
             e = calloc(1, sizeof(Module_Version_Element));
             e->name = eina_stringshare_add(json_cur->valuestring);
             e->cmd = eina_stringshare_printf(version->versions.default_cmd,
                                              e->name);
          }
        else if (json_cur->type == cJSON_Object)
          {
             cJSON *var = json_cur->child;

             e = calloc(1, sizeof(Module_Version_Element));
             e->name = eina_stringshare_add(var->string);
             e->cmd = eina_stringshare_add(var->valuestring);
          }
       else
         goto loop_next;

         NFO("Found element “%s” using cmd “%s”", e->name, e->cmd);
         l = eina_list_append(l, e);

loop_next:
        json_cur = json_cur->next;
     }
   version->versions.list = l;
}

/**
 * @brief Load conf for Alfred.
 * Get the software list for wich we'll want the version n°.
 * @param version Module object
 */
void
version_alfred_conf_load(Module_Version *version)
{
   cJSON *json;

   json = gotham_modules_conf_load(MODULE_VERSION_CONF);
   if (!json)
     return;

   /* Load custom variables for pattern search */
   _version_conf_pattern_vars_load(version, json);
   _version_conf_versions_list_load(version, json);

   cJSON_Delete(json);
}

/**
 * @brief Load conf for Botman.
 * There is a defaut command to get a software version. But in some cases,
 * we need a custom command to get it.
 * So we first load the default command, then we get the software list, in which
 * some of them will have their own command to run.
 * @param version Module object
 */
void
version_botman_conf_load(Module_Version *version)
{
   cJSON *json;

   json = gotham_modules_conf_load(MODULE_VERSION_CONF);
   if (!json)
     return;

   /* Load default command */
   _version_conf_default_command_load(version, json);

   /* Load software list and associated custom commands */
   _version_conf_versions_list_load(version, json);

   cJSON_Delete(json);
}

/**
 * @}
 */
