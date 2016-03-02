#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "seen.h"
#include <fnmatch.h>

const char *
_seen_query_citizen_print(Module_Seen *seen,
                          Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   Eina_Array_Iterator it;
   unsigned int i;
   const char *item,
              *ptr,
              *last_time;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "\t%s %s ",
                             (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE) ?
                                "offline" : "online",
                             citizen->jid);

   EINA_ARRAY_ITER_NEXT(seen->vars, i, item, it)
     {
        const char *var = VARGET(item);

        if (!var)
          continue;

        eina_strbuf_append_printf(buf, "%s[%s] ", item, var);
     }

   if (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE)
     {
        double timestamp;
        const char *seen_last = NULL;

        seen_last = VARGET("seen_last");

        timestamp = time(0) - ((seen_last) ? atof(seen_last) : 0);
        last_time = seen_utils_elapsed_time(timestamp);
        eina_strbuf_append_printf(buf, "time %s", last_time);
        free((char *)last_time);
     }

   eina_strbuf_append(buf, "\n");

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

const char *
seen_query(Module_Seen *seen,
           const char *pattern)
{
   Eina_Strbuf *b;
   Eina_List *l_citizen,
             *l;
   Gotham_Citizen *citizen;
   const char *s;

   b = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(b, NULL);

   eina_strbuf_append_printf(b, "\nContacts that matched :\n");

   l_citizen = gotham_citizen_match(seen->gotham, pattern,
                                    GOTHAM_CITIZEN_TYPE_BOTMAN, seen->vars);

   EINA_LIST_FOREACH(l_citizen, l, citizen)
     {
        const char *line;

        line = _seen_query_citizen_print(seen, citizen);
        eina_strbuf_append(b, line);
        free((char *)line);
     }

   s = eina_list_count(l_citizen) ? eina_strbuf_string_steal(b) :
                                    strdup("No one matches given pattern");
   eina_strbuf_free(b);
   eina_list_free(l_citizen);
   return s;
}
