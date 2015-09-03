#include "info.h"

static const char *name = "Info";

void *
module_register(Gotham *gotham)
{
   Module_Info *info;

   info = calloc(1, sizeof(Module_Info));
   EINA_SAFETY_ON_NULL_RETURN_VAL(info, NULL);

   DBG("info[%p]", info);

   info->gotham = gotham;

   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     info_conf_alfred_load(info);

   return info;
}

void
module_unregister(void *data)
{
   Module_Info *info = data;
   char *s;
   unsigned int i;
   Eina_Array_Iterator iterator;

   DBG("info[%p]", info);

   EINA_ARRAY_ITER_NEXT(info->search_vars, i, s, iterator)
     free(s);
   eina_array_free(info->search_vars);

   free(info);
}

void *
module_init(void)
{
   eina_init();

   _module_info_log_dom = eina_log_domain_register("module_info", EINA_COLOR_YELLOW);

   return (void *)name;
}

void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_info_log_dom);
   eina_shutdown();
}
