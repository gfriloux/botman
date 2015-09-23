#include "info.h"

struct _Alfred_Sort
{
   Module_Info *info;

   Eina_Hash *groups;
};

static void
_alfred_sort_free_cb(void *data)
{
   Eina_List *l = data;
   eina_list_free(l);
}

char *
alfred_sort_print(Alfred_Sort *as,
                  const char *operation)
{
   Eina_Iterator *it;
   void *data;
   Eina_Strbuf *buf;
   char *s;

   buf = eina_strbuf_new();

   eina_strbuf_append(buf, "\n");

   it = eina_hash_iterator_tuple_new(as->groups);
   while (eina_iterator_next(it, &data))
     {
        Eina_Hash_Tuple *t = data;
        const char *name = t->key;
        Eina_List *l = t->data,
                  *l2;
        Gotham_Citizen *citizen;

        eina_strbuf_append_printf(buf, "%s : %s\n", operation, name);

        EINA_LIST_FOREACH(l, l2, citizen)
          {
             const char *p;

             p = utils_citizen_print(as->info, citizen);
             eina_strbuf_append_printf(buf, "\t\t%s\n", p);
             free((char *)p);
          }
        eina_strbuf_append(buf, "\n");
     }
   eina_iterator_free(it);

   s = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return s;
}

void
alfred_sort_add(Alfred_Sort *as,
                const char *value,
                Gotham_Citizen *citizen)
{
   Eina_List *l,
             *lf;

   lf = eina_hash_find(as->groups, value);
DBG("lf[%p]", lf);

   l = eina_list_append(lf, citizen);
   if (l != lf) eina_hash_add(as->groups, value, l);

DBG("l[%p]", l);
}

Alfred_Sort *
alfred_sort_new(Module_Info *info)
{
   Alfred_Sort *as;

   as = calloc(1, sizeof(Alfred_Sort));
   EINA_SAFETY_ON_NULL_RETURN_VAL(as, NULL);

   as->groups = eina_hash_string_small_new(_alfred_sort_free_cb);

   as->info = info;

   return as;
}

void
alfred_sort_free(Alfred_Sort *as)
{
   eina_hash_free(as->groups);
   free(as);
}
