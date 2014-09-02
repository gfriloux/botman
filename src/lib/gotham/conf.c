#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Eet.h>
#include <Gotham.h>
#include <cJSON.h>

#include "gotham_private.h"

/**
 * @addtogroup Libgotham_Conf_Functions
 * @{
 */

/**
 * Reads the xmpp part from a json object, and store values in Gotham_Conf
 * structure
 * @param conf Gotham_Conf to fill
 * @param xmpp Json object representing the xmpp settigns
 * @return EINA_TRUE
 */
Eina_Bool
_gotham_conf_xmpp_read(Gotham_Conf *conf, cJSON *xmpp)
{
#define _LOOKUP(p) {                                                           \
   for (i = 0; lkt[i].key; i++)                                                \
     {                                                                         \
        if (strcmp(lkt[i].key, p->string)) continue;                           \
        *(lkt[i].ptr) = eina_stringshare_add(p->valuestring);                  \
     }                                                                         \
}

   unsigned int i;
   struct _Lookup_Conf
   {
      const char **ptr,
                  *key;
      Eina_Bool mandatory;
   };
   struct _Lookup_Conf lkt[] = {
      { &conf->xmpp.server    , "server"     , EINA_TRUE    },
      { &conf->xmpp.login     , "login"      , EINA_TRUE    },
      { &conf->xmpp.passwd    , "passwd"     , EINA_TRUE    },
      { &conf->xmpp.resource  , "resource"   , EINA_TRUE    },
      { &conf->xmpp.alfred    , "alfred"     , EINA_FALSE   },
      { NULL                  , NULL         , EINA_FALSE   }
   };

   xmpp = xmpp->child;
   while (1)
     {
        if (!xmpp) break;
        _LOOKUP(xmpp);
        xmpp = xmpp->next;
     }
   return EINA_TRUE;
#undef _LOOKUP
}

/**
 * Runs through a json array and return each value as an Eina_List
 * @param list Eina_List pointer
 * @param json Json array to read
 * @return EINA_TRUE
 */
Eina_Bool
_gotham_conf_list_read(Eina_List **list, cJSON *json)
{
   unsigned int i,
                size;

   size = cJSON_GetArraySize(json);
   for (i=0; i<size; i++)
     {
        cJSON *var = cJSON_GetArrayItem(json, i);
        *list = eina_list_append(*list, strdup(var->valuestring));
     }

   return EINA_TRUE;
}

/**
 * Free a Gotham_Conf structure
 * @param conf Gotham_Conf structure to free
 */
void
_gotham_conf_free(Gotham_Conf *conf)
{
   char *tmp;
   DBG("conf[%p]", conf);

   EINA_SAFETY_ON_NULL_RETURN(conf);

#define _FREE(a) if (conf->a) eina_stringshare_del(conf->a);
   _FREE(xmpp.server);
   _FREE(xmpp.login);
   _FREE(xmpp.passwd);
   _FREE(xmpp.resource);
   _FREE(xmpp.alfred);
#undef _FREE

   EINA_LIST_FREE(conf->modules, tmp)
     free(tmp);

   EINA_LIST_FREE(conf->friends, tmp)
     free(tmp);

   free(conf);
}

/**
 * Read file and return its content in a char*
 * @param conf_file File to read
 * @return Conf file's content
 */
char *
_gotham_conf_file_read(const char *conf_file)
{
   Eina_File *fh;
   const char *data;
   char *conf;

   fh = eina_file_open(conf_file, EINA_FALSE);
   if (!fh)
     {
        NFO("Unable to open file “%s”", conf_file);
        return NULL;
     }

   data = (const char *)eina_file_map_all(fh, EINA_FILE_SEQUENTIAL);
   conf = strdup(data);
   eina_file_map_free(fh, (void *)data);
   eina_file_close(fh);

   return conf;
}

/**
 * Read and parse conf file (JSON) and store values in a new
 * Gotham_Conf structure
 * @param conf_file File to load
 * @return Newly created Gotham_Conf structure
 */
Gotham_Conf *
_gotham_conf_load(const char *conf_file)
{
   Gotham_Conf *conf = calloc(1, sizeof(Gotham_Conf));
   cJSON *json = NULL,
         *json_xmpp,
         *json_modules,
         *json_friends;
   char *ptr;
   Eina_Bool ret = EINA_FALSE;

   DBG("conf_file=%s", conf_file);

   ptr = _gotham_conf_file_read(conf_file);
   if (!ptr)
     {
        ERR("Failed to read file “%s”", conf_file);
        return NULL;
     }
   json = cJSON_Parse(ptr);
   if (!json)
     {
        ERR("Unable to open or parse file “%s”", conf_file);
        free(conf);
        return NULL;
     }
   free(ptr);

   json_xmpp = cJSON_GetObjectItem(json, "xmpp");
   if (!json_xmpp)
     {
        ERR("Unable to find xmpp settings in conf file “%s”", conf_file);
        _gotham_conf_free(conf);
        conf = NULL;
        goto func_end;
     }
   ret = _gotham_conf_xmpp_read(conf, json_xmpp);
   if (!ret)
     {
        ERR("Error while loading xmpp settings");
        _gotham_conf_free(conf);
        conf = NULL;
        goto func_end;
     }

   json_modules = cJSON_GetObjectItem(json, "modules");
   if (!json_modules)
     {
        ERR("Unable to find module list in conf file “%s”", conf_file);
        _gotham_conf_free(conf);
        conf = NULL;
        goto func_end;
     }
   ret = _gotham_conf_list_read(&conf->modules, json_modules);
   if (!ret)
     {
        ERR("Error while loading modules list");
        _gotham_conf_free(conf);
        conf = NULL;
        goto func_end;
     }

   json_friends = cJSON_GetObjectItem(json, "friends");
   if (!json_friends)
     {
        ERR("Unable to find friends list in conf file “%s”", conf_file);
        _gotham_conf_free(conf);
        conf = NULL;
        goto func_end;
     }
   ret = _gotham_conf_list_read(&conf->friends, json_friends);
   if (!ret)
     {
        ERR("Error while loading friends list");
        _gotham_conf_free(conf);
        conf = NULL;
        goto func_end;
     }

   DBG("Conf loaded successfully : %d modules, %d friends",
       eina_list_count(conf->modules), eina_list_count(conf->friends));
func_end:
   if (json) cJSON_Delete(json);
   return conf;
}

/**
 * @}
 */

