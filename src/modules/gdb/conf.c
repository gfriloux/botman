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
conf_save(Module_Gdb *gdb)
{
   cJSON *json;
   char *s;
   Eina_List *l;

   json = cJSON_CreateObject();
   EINA_SAFETY_ON_NULL_RETURN(json);

   if (gdb->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        cJSON *dumps;

        dumps = cJSON_CreateArray();
        EINA_SAFETY_ON_NULL_GOTO(dumps, free_json);

        cJSON_AddItemToObject(json, "dumps", dumps);

        EINA_LIST_FOREACH(gdb->dumps.known, l, s)
          cJSON_AddItemReferenceToArray(dumps, cJSON_CreateString(s));
        gotham_modules_conf_save(MODULE_GDB_SAVE, json);
     }
   else
     {
        cJSON *heroes;
        unsigned int i;
        Eina_Array_Iterator iterator;
        const char *heroe;

        heroes = cJSON_CreateArray();
        EINA_SAFETY_ON_NULL_GOTO(heroes, free_json);

        cJSON_AddItemToObject(json, "heroes", heroes);

        EINA_ARRAY_ITER_NEXT(gdb->heroes, i, heroe, iterator)
          cJSON_AddItemReferenceToArray(heroes, cJSON_CreateString(heroe));
        gotham_modules_conf_save(MODULE_GDB_CONF, json);
     }
   cJSON_Delete(json);
   return;

free_json:
   cJSON_Delete(json);
}

void
_conf_load_heroes(Module_Gdb *gdb,
                  cJSON *json_array)
{
   int size,
       i;

   size = cJSON_GetArraySize(json_array);

   gdb->heroes = eina_array_new(2);

   for (i=0; i<size; i++)
     {
        cJSON *hero = cJSON_GetArrayItem(json_array, i);
        DBG("Adding %s", hero->valuestring);
        eina_array_push(gdb->heroes, eina_stringshare_add(hero->valuestring));
     }
}

void
conf_load(Module_Gdb *gdb)
{
   cJSON *json;

   json = gotham_modules_conf_load(MODULE_GDB_CONF);
   EINA_SAFETY_ON_NULL_RETURN(json);

   if (gdb->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        cJSON *dir;

        dir = cJSON_GetObjectItem(json, "dir");
        EINA_SAFETY_ON_NULL_GOTO(dir, free_json);

        gdb->dumps.dir = strdup(dir->valuestring);
     }
   else
     {
        cJSON *heroes;

        heroes = cJSON_GetObjectItem(json, "heroes");
        _conf_load_heroes(gdb, heroes);
     }
   cJSON_Delete(json);

   return;

free_json:
   cJSON_Delete(json);
}
