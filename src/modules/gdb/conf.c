#include "gdb.h"

void
_conf_backup_dumps_load(Module_Gdb *gdb,
                        cJSON *array)
{
   int size,
       i;

   size = cJSON_GetArraySize(array);

   for (i = 0; i < size; i++)
     {
        cJSON *dump = cJSON_GetArrayItem(array, i);
        gdb->dumps.known = eina_list_append(gdb->dumps.known, strdup(dump->valuestring));
     }
}

void
conf_backup_load(Module_Gdb *gdb)
{
   cJSON *json,
         *json_dumps;

   json = gotham_modules_conf_load(MODULE_GDB_SAVE);
   if (!json) return;

   json_dumps = cJSON_GetObjectItem(json, "dumps");

   _conf_backup_dumps_load(gdb, json_dumps);
}

void
conf_backup_save(Module_Gdb *gdb)
{
   cJSON *json,
         *json_dumps;
   char *s;
   Eina_List *l;

   json = cJSON_CreateObject();
   EINA_SAFETY_ON_NULL_RETURN(json);

   json_dumps = cJSON_CreateArray();
   EINA_SAFETY_ON_NULL_GOTO(json_dumps, free_json);

   cJSON_AddItemToObject(json, "dumps", json_dumps);

   EINA_LIST_FOREACH(gdb->dumps.known, l, s)
     {
        cJSON_AddItemReferenceToArray(json_dumps, cJSON_CreateString(s));
     }

   gotham_modules_conf_save(MODULE_GDB_SAVE, json);
   cJSON_Delete(json);
   return;

free_json:
   cJSON_Delete(json);
}

void
conf_load(Module_Gdb *gdb)
{
   cJSON *json,
         *json_dir;

   json = gotham_modules_conf_load(MODULE_GDB_CONF);
   EINA_SAFETY_ON_NULL_RETURN(json);

   json_dir = cJSON_GetObjectItem(json, "dir");
   EINA_SAFETY_ON_NULL_GOTO(json_dir, free_json);

   gdb->dumps.dir = strdup(json_dir->valuestring);

   cJSON_Delete(json);

   return;

free_json:
   cJSON_Delete(json);
}
