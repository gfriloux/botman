#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE
#include <Eina.h>
#include <Gotham.h>
#include <fnmatch.h>
#include "gotham_private.h"

/**
 * @addtogroup Libgotham_Citizen_Functions
 * @{
 */

Eina_Iterator *
gotham_citizen_var_iterate(Gotham_Citizen *citizen)
{
   return (Eina_Iterator *)eina_hash_iterator_tuple_new(citizen->vars);
}

/**
 * Free a custm var data
 * @param data Data to free
 */
static void
_gotham_citizen_var_free(void *data)
{
   free((char *)data);
}

/**
 * Creates a new gotham_citizen
 * @param gotham Gotham structure
 * @param jid Citizen's jid
 * @return Gotham_Citizen Newly created structure
 */
Gotham_Citizen *
gotham_citizen_new(Gotham *gotham, const char *jid)
{
   Gotham_Citizen *citizen = NULL;

   DBG("gotham[%p] jid[%p]=%s", gotham, jid, jid);
   EINA_SAFETY_ON_NULL_RETURN_VAL(jid, NULL);
   citizen = calloc(1 , sizeof(Gotham_Citizen));

   citizen->jid = eina_stringshare_add(jid);

   if ((!gotham->conf->xmpp.alfred) ||
       (!strncmp(gotham->conf->xmpp.alfred, jid, strlen(gotham->conf->xmpp.alfred))))
     citizen->type = GOTHAM_CITIZEN_TYPE_ALFRED;
   else
     citizen->type = GOTHAM_CITIZEN_TYPE_UNIDENTIFIED;

   citizen->status = GOTHAM_CITIZEN_STATUS_OFFLINE;
   citizen->subscribed = EINA_FALSE;
   citizen->gotham = gotham;
   citizen->vars = eina_hash_string_superfast_new(_gotham_citizen_var_free);
   DBG("citizen[%p] citizen->jid[%s] citizen->type[%d]", citizen, citizen->jid, citizen->type);
   return citizen;
}

/**
 * Free a Gotham_Citizen
 * @param citizen Gotham_Citizen to free
 */
void
gotham_citizen_free(Gotham_Citizen *citizen)
{
   EINA_SAFETY_ON_NULL_RETURN(citizen);
   DBG("citizen[%p]", citizen);
   eina_stringshare_replace(&citizen->jid, NULL);
   eina_stringshare_replace(&citizen->nickname, NULL);
   eina_hash_free(citizen->vars);
   free(citizen);
}

/**
 * Search in citizen list if the given jid matches a known citizen
 * @param gotham Gotham structure
 * @param jid Citizen jid to search
 * @return EINA_TRUE if citizen is found, otherwise EINA_FALSE
 */
Eina_Bool
gotham_citizen_exist(Gotham *gotham, const char *jid)
{
   return eina_hash_find(gotham->citizens, jid) ? EINA_TRUE : EINA_FALSE;
}

/**
 * Sends a message to a citizen
 * @param citizen Gotham_Citizen who will receive the message
 * @param msg Message to send
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_citizen_send(Gotham_Citizen *citizen, const char *msg)
{
   DBG("Sending message from %s to %s",
       citizen->gotham->me->jid, citizen->jid);

   /* debug */
   if (!strcmp(msg, "Access denied"))
     {
        ERR("*** Access denied *** from %s to %s",
            citizen->gotham->me->jid, citizen->jid);
     }

   return shotgun_message_send(citizen->gotham->shotgun,
                               citizen->jid,
                               msg,
                               SHOTGUN_MESSAGE_STATUS_ACTIVE,
                               (citizen->features.xhtml) ?
                                 EINA_TRUE :EINA_FALSE);
}

/**
 * Callback for freeing citizen hash list (called on shutdown)
 * @param data Gotham_Citizen to free
 */
void
_gotham_citizen_list_free_cb(void *data)
{
   Gotham_Citizen *citizen = data;
   gotham_citizen_free(citizen);
}

/**
 * Deletes a variable from a citizen's hash list
 * @param citizen Gotham_Citizen
 * @param name Variable to delete
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_citizen_var_del(Gotham_Citizen *citizen,
                       const char *name)
{
   DBG("name=%s", name);
   return eina_hash_del_by_key(citizen->vars, name);
}

/**
 * Add or update a citizen's variable in his hash list
 * @param citizen Gotham_Citizen
 * @param name Variable name
 * @param value Value to set. If value is a format, __VA_ARGS__
 *              must be provided
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_citizen_var_set(Gotham_Citizen *citizen,
                       const char *name,
                       const char *value,
                       ...)
{
   va_list args;
   char *ptr,
        *str;
   size_t len;

   va_start(args, value);
   len = vasprintf(&str, value, args);
   va_end(args);

   DBG("name=%s value=%s str=%s", name, value, str);

   if (len <=  0 || !str)
     return EINA_FALSE;

   ptr = (char *)eina_hash_modify(citizen->vars, name, str);
   if (!ptr)
     return eina_hash_add(citizen->vars, name, str);

   free((char *)ptr);
   return EINA_TRUE;
}

/**
 * Get a citizen's variable
 * @param citizen Gotham_Citizen
 * @param name Variable name to search for
 * @return Value matching given name, or null if not found
 */
const char *
gotham_citizen_var_get(Gotham_Citizen *citizen,
                       const char *name)
{
   return (const char *)eina_hash_find(citizen->vars, name);
}


/**
 * Returns a citizen list that matches some criterias.
 * Provided pattern can match citizen's jid, or one on its custom variables
 * Returning Eina_List has to be freed when unused, but you should never
 * free its content
 * @param gotham Gotham structure
 * @param pattern Pattern to search
 * @param type Gotham_Citizen_Type
 * @param vars Eina_Array containing each citizen var we'll be looking in
 * @return Eina_List containing every wanted citizen
 */
Eina_List *
gotham_citizen_match(Gotham *gotham,
                     const char *pattern,
                     Gotham_Citizen_Type type,
                     Eina_List *vars)
{
   Gotham_Citizen *citizen;
   Eina_Iterator *it;
   void *it_data;
   Eina_List *l = NULL;

   DBG("gotham[%p], pattern[%p]:“%s”, type=%d, vars[%p]",
       gotham, pattern, pattern, type, vars);

   it = eina_hash_iterator_data_new(gotham->citizens);
   while (eina_iterator_next(it, &it_data))
     {
        citizen = it_data;

        if (type != citizen->type)
          continue;

        if (!_gotham_citizen_vars_match(vars, citizen, pattern))
          continue;

        DBG("Citizen “%s” matches pattern", citizen->jid);
        l = eina_list_append(l, citizen);
     }
   eina_iterator_free(it);

   return l;
}


/**
 * Checks if a citizen has a custom var that matches the search pattern
 * @param vars Eina_Array containing vars name to search in
 * @param citizen Gotham_Citizen
 * @param pattern Pattern to match
 * @return EINA_TRUE if citizen has a var matching pattern,
           otherwise EINA_FALSE
 */
Eina_Bool
_gotham_citizen_vars_match(Eina_List *vars,
                           Gotham_Citizen *citizen,
                           const char *pattern)
{
   Eina_List *l;
   const char *item;

   if (!fnmatch(pattern, citizen->jid, FNM_NOESCAPE))
     return EINA_TRUE;

   EINA_LIST_FOREACH(vars, l, item)
     {
        const char *var = gotham_citizen_var_get(citizen, item);

        if (!var) continue;

        if (!fnmatch(pattern, var, FNM_NOESCAPE))
          return EINA_TRUE;
     }

   return EINA_FALSE;
}

/**
 * @brief Add a line to result ("citizen that match" part).
 * When a citizen matches the search pattern, add a line to result
 * with citizen and some informations : status (on/offline), softwares
 * licence number, bunker.
 * @param obj Module object
 * @param citizen Gotham_Citizen to print
 * @return const char * line to add to result
 */
const char *
citizen_match_print(Eina_List *vars,
                    Gotham_Citizen *citizen,
                    Eina_Bool print_presence)
{
   Eina_Strbuf *buf;
   Eina_List *l;
   const char *item,
              *ptr;

   buf = eina_strbuf_new();

   if (print_presence)
     eina_strbuf_append_printf(buf, "%s %s ",
                               (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE) ?
                                  "offline" : "online",
                               citizen->jid);

   EINA_LIST_FOREACH(vars, l, item)
     {
        const char *var = gotham_citizen_var_get(citizen, item);

        if (!var) continue;
        eina_strbuf_append_printf(buf, "%s[%s] ", item, var);
     }

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

/**
 * Checks if a citizen is allowed to talk with us.
 * Allowed citizen are defined in gotham conf file
 * @param gotham Gotham structure
 * @param jid Citizen's jid to check
 * @return EINA_TRUE if citizen is auth, otherwise EINA_FALSE
 */
Eina_Bool
_gotham_citizen_authorized(Gotham *gotham,
                           const char *jid)
{
   Eina_List *l;
   const char *pattern;
   Eina_Bool found = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(gotham, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(jid, EINA_FALSE);

   if ((gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
       (!strcmp(jid, gotham->alfred->jid)))
     return EINA_TRUE;

   EINA_LIST_FOREACH(gotham->conf->friends, l, pattern)
     {
        if (fnmatch(pattern, jid, FNM_NOESCAPE)) continue;

        found = EINA_TRUE;
        break;
     }

   return found;
}

/**
 * @}
 */
