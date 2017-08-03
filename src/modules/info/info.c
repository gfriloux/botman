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
     {
        info->conf = gotham_serialize_file_to_struct(MODULE_INFO_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Info_Conf);
        info_alfred_commands_register();
     }
   else
     info->sync = ecore_timer_add(60, info_botman_sync, info);

   return info;
}

void
module_unregister(void *data)
{
   Module_Info *info = data;

   DBG("info[%p]", info);

   if (info->conf) Module_Info_Conf_free(info->conf);
   if (info->sync) ecore_timer_del(info->sync);

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
