#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "spam.h"


Eina_List *
spam_query(Module_Spam *spam,
           const char *pattern,
           const char *message)
{
   Eina_List *l_citizen,
             *l;
   Gotham_Citizen *citizen;

   l_citizen = gotham_citizen_match(spam->gotham, pattern,
                                    GOTHAM_CITIZEN_TYPE_BOTMAN, spam->conf->vars);


   EINA_LIST_FOREACH(l_citizen, l, citizen)
     gotham_citizen_send(citizen, message);

   return l_citizen;
}
