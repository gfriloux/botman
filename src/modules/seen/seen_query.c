#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "seen.h"
#include <fnmatch.h>

/* This function has been added to have the HTTPd module
 * able to expose a /seen URI answering JSON data instead
 * of plaintext.
 */
Eina_List *
seen_query_match(Gotham *gotham,
                 const char *pattern,
                 Gotham_Citizen_Type citizen_type,
                 Module_Seen *seen)
{
   return gotham_citizen_match(gotham, pattern, citizen_type, seen->conf->vars);
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
                                    GOTHAM_CITIZEN_TYPE_BOTMAN, seen->conf->vars);

   EINA_LIST_FOREACH(l_citizen, l, citizen)
     {
        const char *line;

        line = gotham_citizen_match_print(seen->conf->vars, citizen, EINA_TRUE, EINA_TRUE);
        eina_strbuf_append(b, line);
        free((char *)line);
     }

   s = eina_list_count(l_citizen) ? eina_strbuf_string_steal(b) :
                                    strdup("No one matches given pattern");
   eina_strbuf_free(b);
   eina_list_free(l_citizen);
   return s;
}
