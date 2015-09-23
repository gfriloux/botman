#include "info.h"

const char *
utils_citizen_print(Module_Info *info,
                    Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   Eina_Array_Iterator it;
   unsigned int i;
   const char *item,
              *ptr;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "%s %s ",
                             (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE) ?
                                "offline" : "online",
                             citizen->jid);

   EINA_ARRAY_ITER_NEXT(info->search_vars, i, item, it)
     {
        const char *var = VARGET(item);

        if (!var)
          continue;

        eina_strbuf_append_printf(buf, "%s[%s] ", item, var);
     }

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}
