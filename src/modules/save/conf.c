#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cJSON.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "save.h"

/**
 * @addtogroup Gotham_Module_Save
 * @{
 */

/**
 * @brief Backup vars stored in memory into a JSON object.
 * @param json cJSON object to fill
 * @param list Eina_Hash list containing vars to backup.
 */
void
_conf_backup_vars(cJSON *json, Eina_Hash *list)
{
   Eina_Iterator *it;
   void *it_data;

   it = eina_hash_iterator_tuple_new(list);
   while (eina_iterator_next(it, &it_data))
     {
        Eina_Hash_Tuple *t = it_data;
        cJSON_AddItemToObject(json, t->key, cJSON_CreateString(t->data));
     }
   eina_iterator_free(it);
}

/**
 * @brief Backup a citizen into a JSON object.
 * Citizen will be backuped with all his custom vars
 * @param citizen Gotham_Citizen to backup
 * @return cJSON object representing citizen
 */
cJSON *
_conf_backup_citizens_citizen(Gotham_Citizen *citizen)
{
#define _ADD(_a, _b, _c, _d)                                                   \
   cJSON_AddItemToObject(json_##_a, _b, cJSON_Create##_c(_d))
   cJSON *json_citizen = NULL,
         *json_features = NULL,
         *json_vars = NULL;

   json_citizen = cJSON_CreateObject();
   json_features = cJSON_CreateObject();
   json_vars = cJSON_CreateObject();

   if ((!json_citizen) || (!json_features) || (!json_vars))
     {
        cJSON_Delete(json_citizen);
        cJSON_Delete(json_features);
        cJSON_Delete(json_vars);
        return NULL;
     }

   cJSON_AddItemToObject(json_citizen, "features", json_features);
   cJSON_AddItemToObject(json_citizen, "vars", json_vars);

   _ADD(citizen, "jid", String, citizen->jid);
   _ADD(citizen, "nickname", String,
        (citizen->nickname) ? citizen->nickname : "");
   _ADD(citizen, "type", Number, (double)citizen->type);
   _ADD(citizen, "status", Number, (double)citizen->status);
   _ADD(features, "xhtml", Number, (double)citizen->features.xhtml);
   _ADD(features, "last", Number, (double)citizen->features.last);

   _conf_backup_vars(json_vars, citizen->vars);
   return json_citizen;
#undef _ADD
}

/**
 * @brief Backup all citizen into JSON object.
 * For each citizen, call @ref _conf_backup_citizens_citizen
 * @param json cJSON object to fill
 * @param citizens Eina_Hash list containing citizens to backup
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
_conf_backup_citizens(cJSON *json, Eina_Hash *citizens)
{
   Gotham_Citizen *citizen;
   Eina_Iterator *it;
   void *it_data;

   it = eina_hash_iterator_data_new(citizens);
   while (eina_iterator_next(it, &it_data))
     {
        citizen = it_data;
        cJSON *json_citizen;

        json_citizen = _conf_backup_citizens_citizen(citizen);
        if (!json_citizen)
          {
             eina_iterator_free(it);
             return EINA_FALSE;
          }
        cJSON_AddItemToArray(json, json_citizen);
     }
   eina_iterator_free(it);

   return EINA_TRUE;
}

/**
 * @brief Backup a module's command list into a JSON object.
 * Run through commands list.
 * Each Command will be backuped with all its custom vars
 * @param json cJSON object to fill
 * @param commands Eina_Inlist commands list
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
_conf_backup_modules_command(cJSON *json, Eina_Inlist *commands)
{
   cJSON *json_command = NULL,
         *json_command_vars = NULL;
   Gotham_Module_Command *command;

   EINA_INLIST_FOREACH(commands, command)
     {
        json_command = cJSON_CreateObject();
        json_command_vars = cJSON_CreateObject();

        if ((!json_command) || (!json_command_vars))
          {
             ERR("Failed to allocate");
             cJSON_Delete(json_command);
             cJSON_Delete(json_command_vars);
             return EINA_FALSE;
          }
        cJSON_AddItemToObject(json_command, "name",
                              cJSON_CreateString(command->command));

        _conf_backup_vars(json_command_vars, command->vars);
        cJSON_AddItemToObject(json_command, "vars", json_command_vars);
        cJSON_AddItemToArray(json, json_command);

     }
   return EINA_TRUE;
}

/**
 * @brief Backup all modules into a JSON object.
 * For each module, create a new json object, then
 * call @ref _conf_backup_modules_command for module commands
 * @param json cJSON object to fill
 * @param modules Eina_Hash list containing modules to backup
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
_conf_backup_modules(cJSON *json, Eina_Inlist *modules)
{
   Gotham_Module *module;
   cJSON *json_module,
         *json_commands;

   EINA_INLIST_FOREACH(modules, module)
     {
        json_module = cJSON_CreateObject();
        json_commands = cJSON_CreateArray();

        if ((!json_module) || (!json_commands))
          {
             ERR("Failed to allocate");
             cJSON_Delete(json_module);
             cJSON_Delete(json_commands);
             return EINA_FALSE;
          }

        cJSON_AddItemToObject(json_module, "name",
                              cJSON_CreateString(module->name));
        _conf_backup_modules_command(json_commands, module->commands);
        cJSON_AddItemToObject(json_module, "commands", json_commands);
        cJSON_AddItemToArray(json, json_module);
     }
   return EINA_TRUE;
}

/**
 * @brief General backuping function.
 * Make a backup of memory into file. Backup will be in JSON format and
 * contains :
 * @li me : Gotham_Citizen representing current instance
 * @li citizens : List of Gotham_Citizen for each firend
 * @li modules : List of Gotham_Modules
 * @param data Module_Save object
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
conf_backup(void *data)
{
#define _FREEALL
   Module_Save *save = data;
   cJSON *json = NULL,
         *json_me = NULL,
         *json_citizens = NULL,
         *json_modules = NULL;
   char *json_text,
        file[PATH_MAX],
        file_tmp[PATH_MAX];
   int fd;
   ssize_t size = 0,
           offset = 0,
           nb = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_FALSE);

   json = cJSON_CreateObject();
   //json_me = cJSON_CreateArray();
   json_citizens = cJSON_CreateArray();
   json_modules = cJSON_CreateArray();

   if ((!json) || (!json_citizens) || (!json_modules))
     {
        ERR("Failed to create json object");
        cJSON_Delete(json);
        cJSON_Delete(json_citizens);
        cJSON_Delete(json_modules);
        return EINA_FALSE;
     }

   json_me = _conf_backup_citizens_citizen(save->gotham->me);
   if (!json_me)
     {
        ERR("Failed to save me");
        cJSON_Delete(json);
        return EINA_TRUE;
     }
   cJSON_AddItemToObject(json, "me", json_me);
   cJSON_AddItemToObject(json, "citizens", json_citizens);
   cJSON_AddItemToObject(json, "modules", json_modules);

   if (!_conf_backup_citizens(json_citizens, save->gotham->citizens))
     {
        ERR("Failed to save citizens");
        cJSON_Delete(json);
        return EINA_TRUE;
     }

   if (!_conf_backup_modules(json_modules, gotham_modules_list()))
     {
        ERR("Failed to save modules");
        cJSON_Delete(json);
        return EINA_TRUE;
     }

   json_text = cJSON_Print(json);
   cJSON_Delete(json);
   snprintf(file, sizeof(file), "%s%.64s.%.255s.save", MODULE_SAVE_BACKUP,
            save->gotham->conf->xmpp->login,
            save->gotham->conf->xmpp->server);
   snprintf(file_tmp, sizeof(file), "%s.tmp", file);

   fd = open(file_tmp, O_WRONLY | O_CREAT | O_TRUNC, 0700);
   if (fd == -1)
     {
        ERR("Failed to open %s : %s", file_tmp, strerror(errno));
        free(json_text);
        return EINA_TRUE;
     }

   size = strlen(json_text);
   while (offset != size)
     {
        nb = write(fd, json_text + offset, size - offset);
        if (nb == -1)
          {
             ERR("Failed to write to %s : %s", file_tmp, strerror(errno));
             free(json_text);
             close(fd);
             return EINA_TRUE;
          }
        offset += nb;
     }
   free(json_text);
   close(fd);

   if (rename(file_tmp, file))
     {
        ERR("Failed to rename %s to %s : %s", file_tmp, file, strerror(errno));
     }

   return EINA_TRUE;
}

/**
 * @brief Restore a citizen's custom vars from JSON to Gotham_Citizen obj.
 * @param citizen Gotham_Citizen to fill.
 * @param json cJSON object to read
 */
void
_conf_restore_citizens_vars(Gotham_Citizen *citizen,
                            cJSON *json)
{
   int vars_count,
       i;
   DBG("citizen[%p]=%s json[%p]", citizen, citizen->jid, json);

   EINA_SAFETY_ON_NULL_RETURN(citizen);
   EINA_SAFETY_ON_NULL_RETURN(json);

   vars_count = cJSON_GetArraySize(json);

   for (i=0; i<vars_count; i++)
     {
        cJSON *var = cJSON_GetArrayItem(json, i);
        NFO("Found vars %s", var->string);

        if (gotham_citizen_var_get(citizen, var->string))
          continue;

        gotham_citizen_var_set(citizen, var->string, var->valuestring);
     }
}

/**
 * @brief Restore citizen list from JSON to struct.
 * @param json cJSON object to read
 * @param gotham Gotham object to fill
 */
void
_conf_restore_citizens(cJSON *json, Gotham *gotham)
{
#define _JSGET(_a, _b, _c) _a = utils_json_##_b(json_citizen, _c)
   cJSON *json_citizen,
         *json_citizen_vars;
   Gotham_Citizen *citizen;
   const char *jid,
              *jid_shared,
              *nickname;
   int type;

   DBG("json[%p] gotham[%p]", json, gotham);

   EINA_SAFETY_ON_NULL_RETURN(json);
   EINA_SAFETY_ON_NULL_RETURN(gotham);

   json_citizen = json->child;

   while (json_citizen)
     {
        _JSGET(jid, string_get, "jid");
        _JSGET(nickname, string_get, "nickname");
        _JSGET(type, number_get, "type");

        jid_shared = eina_stringshare_add(jid);

        citizen = eina_hash_find(gotham->citizens, jid_shared);
        eina_stringshare_del(jid_shared);
        if (!citizen)
          {
             citizen = gotham_citizen_new(gotham, jid);
             eina_hash_direct_add(gotham->citizens, citizen->jid, citizen);
             eina_stringshare_replace(&citizen->nickname, nickname);
          }

        DBG("Citizen JID[%s] Type[%d] Save type[%d]",
            citizen->jid, citizen->type, type);

        if ((Gotham_Citizen_Type)type != citizen->type)
          citizen->type = type;

        /* Gettings vars */
        json_citizen_vars = cJSON_GetObjectItem(json_citizen, "vars");
        _conf_restore_citizens_vars(citizen, json_citizen_vars);

        /*if (!json_citizen->next)
          break;*/

        json_citizen = json_citizen->next;
     }
#undef _JSGET
}

/**
 * @brief Restore modules from JSON to structure.
 * @todo : write this function (does not do anything yet)
 * @param json UNUSED
 * @param gotham UNUSED
 */
void
_conf_restore_modules(cJSON *json EINA_UNUSED,
                      Gotham *gotham EINA_UNUSED)
{
   /* Yet i have no idea when could it be usefull, so doing nothing */
}

/**
 * @brief Restore saved conf into objects.
 * Read backup file, parse it into cJSON object, then restore citizen
 * and vars.
 * @param save Module_Save object
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
conf_restore(Module_Save *save)
{
   Gotham *gotham = save->gotham;
   Eina_File *fh;
   const char *fh_data;
   char file[PATH_MAX];
   cJSON *json,
         *json_me,
         *json_citizens,
         *json_modules;


   snprintf(file, sizeof(file), "%s%.64s.%.255s.save", MODULE_SAVE_BACKUP,
            gotham->conf->xmpp->login, gotham->conf->xmpp->server);
   fh = eina_file_open(file, EINA_FALSE);
   if (!fh)
     {
        NFO("There isnt a backup for this account");
        return EINA_TRUE;
     }

   fh_data = (const char *)eina_file_map_all(fh, EINA_FILE_SEQUENTIAL);
   json = cJSON_Parse(fh_data);
   eina_file_map_free(fh, (void *)fh_data);
   eina_file_close(fh);
   if (!json)
     {
        ERR("Failed to parse configuration file!");
        return EINA_TRUE;
     }

   json_me = cJSON_GetObjectItem(json, "me");
   json_citizens = cJSON_GetObjectItem(json, "citizens");
   json_modules = cJSON_GetObjectItem(json, "modules");
   _conf_restore_citizens_vars(gotham->me, cJSON_GetObjectItem(json_me, "vars"));
   _conf_restore_citizens(json_citizens, gotham);
   _conf_restore_modules(json_modules, gotham);

   cJSON_Delete(json);
   return EINA_TRUE;
}

/**
 * @}
 */
