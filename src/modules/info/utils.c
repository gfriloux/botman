#include "info.h"

const char *
utils_citizen_print(Module_Info *info,
                    Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   Eina_List *l;
   const char *item,
              *ptr;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "%s %s ",
                             (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE) ?
                                "offline" : "online",
                             citizen->jid);

   EINA_LIST_FOREACH(info->conf->search_vars, l, item)
     {
        const char *var = VARGET(item);

        if (!var) continue;
        eina_strbuf_append_printf(buf, "%s[%s] ", item, var);
     }

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}
