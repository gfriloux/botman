#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "alert.h"

static const char *name = "Alert";

void *
module_register(Gotham *gotham)
{
   Module_Alert *alert;
   Eina_List *l;
   Module_Alert_Conf_Command *command;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return NULL;

   alert = calloc(1, sizeof(Module_Alert));
   EINA_SAFETY_ON_NULL_RETURN_VAL(alert, NULL);

   DBG("alert[%p]", alert);

   alert->gotham = gotham;
   alert->conf = gotham_serialize_file_to_struct(MODULE_ALERT_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Alert_Conf);

   EINA_LIST_FOREACH(alert->conf->commands, l, command)
     {
        Module_Alert_Command *mac;

        mac = alert_command_new(alert->gotham, command);
        EINA_SAFETY_ON_NULL_GOTO(mac, end_loop);

        alert->commands = eina_list_append(alert->commands, mac);

end_loop:
        continue;
     }


   return alert;
}

void
module_unregister(void *data)
{
   Module_Alert *alert = data;
   Module_Alert_Command *mac;

   DBG("alert[%p]", alert);

   EINA_LIST_FREE(alert->commands, mac)
     {
        ecore_timer_del(mac->timer);
        free(mac);
     }
   Module_Alert_Conf_free(alert->conf);
   free(alert);
}


const char *
module_init(void)
{
   eina_init();
   _module_alert_log_dom = eina_log_domain_register("module_alert",
                                                    EINA_COLOR_RED);
   return name;
}

void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_alert_log_dom);
   eina_shutdown();
}

