#include "alert.h"

void
_alert_command_clean(Module_Alert_Command *mac)
{
   eina_strbuf_free(mac->buf);

   ecore_event_handler_del(mac->ev.data);
   ecore_event_handler_del(mac->ev.del);
   ecore_event_handler_del(mac->ev.error);
}

Eina_Bool
_alert_command_data(void *data,
                    int type EINA_UNUSED,
                    void *event)
{
   Module_Alert_Command *mac = data;
   Ecore_Exe_Event_Data *d = (Ecore_Exe_Event_Data *)event;
   char *p;

   if (mac != ecore_exe_data_get(d->exe)) return EINA_TRUE;

   DBG("mac[%p] d[%p]", mac, d);

   p = d->data + d->size - 1;
   eina_strbuf_append_length(mac->buf, d->data, *p == '\n' ? d->size-1 : d->size);

   return EINA_TRUE;
}

Eina_Bool
_alert_command_del(void *data,
                   int type EINA_UNUSED,
                   void *event)
{
   Module_Alert_Command *mac = data;
   Ecore_Exe_Event_Del *del = event;
   const char *val;

   if (mac != ecore_exe_data_get(del->exe)) return EINA_TRUE;

   DBG("mac[%p] del[%p]", mac, del);

   val = gotham_citizen_var_get(mac->gotham->me, mac->command->name);
   DBG("val[%s] value[%s]", val, eina_strbuf_string_get(mac->buf));

   if (!val) goto set_var;

   if (strcmp(val, eina_strbuf_string_get(mac->buf)))
     {
        Eina_Strbuf *buf = eina_strbuf_new();
        EINA_SAFETY_ON_NULL_GOTO(buf, clean_mac);

        eina_strbuf_append_printf(buf, ".notification send %s Variable '%s' changed from '%s' to '%s'",
                                  strlen(mac->command->group) ? mac->command->group : "dev",
                                  mac->command->name, val,
                                  eina_strbuf_string_get(mac->buf));
        gotham_citizen_send(mac->gotham->alfred, eina_strbuf_string_get(buf));
        eina_strbuf_free(buf);
     }

set_var:
   gotham_citizen_var_set(mac->gotham->me, mac->command->name, eina_strbuf_string_get(mac->buf));

clean_mac:
   _alert_command_clean(mac);
   return EINA_TRUE;
}

Eina_Bool
_alert_command_error(void *data,
                     int type EINA_UNUSED,
                     void *event)
{
   Module_Alert_Command *mac = data;
   Ecore_Exe_Event_Del *del = event;

   if (mac != ecore_exe_data_get(del->exe)) return EINA_TRUE;

   DBG("mac[%p] del[%p]", mac, del);

   ERR("Execution error");

   _alert_command_clean(mac);
   return EINA_TRUE;
}

Eina_Bool
alert_command_run(void *data)
{
   Module_Alert_Command *mac = data;

#define _EV(_a, _b, _c, _d) _a = ecore_event_handler_add(ECORE_EXE_EVENT_##_b, _c, _d)
   _EV(mac->ev.data, DATA, _alert_command_data, mac);
   EINA_SAFETY_ON_NULL_RETURN_VAL(mac->ev.data, EINA_TRUE);

   _EV(mac->ev.del, DEL, _alert_command_del, mac);
   EINA_SAFETY_ON_NULL_GOTO(mac->ev.del, del_data);

   _EV(mac->ev.error, ERROR, _alert_command_error, mac);
   EINA_SAFETY_ON_NULL_GOTO(mac->ev.error, del_del);
#undef _EV

   mac->buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_GOTO(mac->buf, del_error);

   mac->exe = ecore_exe_pipe_run(mac->command->command, ECORE_EXE_PIPE_READ, mac);
   EINA_SAFETY_ON_NULL_GOTO(mac->exe, free_buf);
   ecore_exe_data_set(mac->exe, mac);

   return EINA_TRUE;

free_buf:
   eina_strbuf_free(mac->buf);
del_error:
   ecore_event_handler_del(mac->ev.error);
del_del:
   ecore_event_handler_del(mac->ev.del);
del_data:
   ecore_event_handler_del(mac->ev.data);
   return EINA_TRUE;
}

Module_Alert_Command *
alert_command_new(Gotham *gotham,
                  Module_Alert_Conf_Command *command)
{
   Module_Alert_Command *mac;

   mac = calloc(1, sizeof(Module_Alert_Command));
   EINA_SAFETY_ON_NULL_RETURN_VAL(mac, NULL);

   mac->gotham = gotham;

   mac->command = command;
   mac->timer = ecore_timer_add(mac->command->interval, alert_command_run, mac);

   return mac;
}
