#include "sysinfo.h"

Module_Sysinfo_Conf_Item *
utils_item_find(Eina_List *list,
                const char *name)
{
   Module_Sysinfo_Conf_Item *item;
   Eina_List *l;

   EINA_LIST_FOREACH(list, l, item)
     {
        if (strcmp(item->name, name)) continue;
        return item;
     }
   return NULL;
}
