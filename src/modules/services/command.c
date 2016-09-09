#include "services.h"

#define _INVALID(_a)                                                           \
   do {                                                                        \
      if (_a->command[2])                                                      \
        break;                                                                 \
      gotham_command_send(_a, "Invalid command");                              \
   } while (0)
/* "debug" */

#define _RUN(_a, _b, _c, _d)                                                   \
   {                                                                           \
      Command_Service *cs;                                                     \
      cs = _command_service_new(_a, _b, _c, _d);                               \
      EINA_SAFETY_ON_NULL_GOTO(cs, error);                                     \
      return;                                                                  \
error:                                                                         \
      gotham_command_send(command, "Memory allocation error");                 \
   } while (0)
/* "debug" */

typedef struct _Command_Service
{
   Module_Services *services;

   const char *jid;
   Eina_Strbuf *buf;
   Ecore_Exe *exe;

   struct
   {
      Ecore_Event_Handler *data,
                          *del,
                          *error;
   } ev;
} Command_Service;

Eina_Bool _command_data(void *data, int type, void *event);
Eina_Bool _command_del(void *data, int type, void *event);
Eina_Bool _command_error(void *data, int type, void *event);

Command_Service *
_command_service_new(Module_Services *services,
                     const char *jid,
                     const char *cmd,
                     const char *service)
{
   Command_Service *cs;
   const char *s;

   DBG("services[%p] jid[%s] cmd[%s] service[%s]",
       services, jid, cmd, service);

   cs = calloc(1, sizeof(Command_Service));
   EINA_SAFETY_ON_NULL_RETURN_VAL(cs, NULL);

   cs->services = services;

   cs->jid = strdup(jid);
   EINA_SAFETY_ON_NULL_GOTO(cs->jid, free_cs);

   cs->buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_GOTO(cs->buf, free_jid);

#define _EV(_a, _b, _c, _d) _a = ecore_event_handler_add(ECORE_EXE_EVENT_##_b, _c, _d)
   _EV(cs->ev.data, DATA, _command_data, cs);
   EINA_SAFETY_ON_NULL_GOTO(cs->ev.data, free_buf);

   _EV(cs->ev.del, DEL, _command_del, cs);
   EINA_SAFETY_ON_NULL_GOTO(cs->ev.del, del_data);

   _EV(cs->ev.error, ERROR, _command_error, cs);
   EINA_SAFETY_ON_NULL_GOTO(cs->ev.error, del_del);
#undef _EV

   s = utils_strdupf(cmd, service);
   EINA_SAFETY_ON_NULL_GOTO(s, del_error);

   DBG("Running command [%s]", s);

   cs->exe = ecore_exe_pipe_run(s, ECORE_EXE_PIPE_READ, cs);
   EINA_SAFETY_ON_NULL_GOTO(cs->exe, free_s);

   ecore_exe_data_set(cs->exe, cs);
   free((char *)s);
   return cs;

free_s:
   free((char *)s);
del_error:
   ecore_event_handler_del(cs->ev.error);
del_del:
   ecore_event_handler_del(cs->ev.del);
del_data:
   ecore_event_handler_del(cs->ev.data);
free_buf:
   eina_strbuf_free(cs->buf);
free_jid:
   free((char *)cs->jid);
free_cs:
   free(cs);
   return NULL;
}

void
_command_service_free(Command_Service *cs)
{
   ecore_event_handler_del(cs->ev.error);
   ecore_event_handler_del(cs->ev.del);
   ecore_event_handler_del(cs->ev.data);

   eina_strbuf_free(cs->buf);
   free((char *)cs->jid);
   free(cs);
}

Eina_Bool
_command_data(void *data,
              int type EINA_UNUSED,
              void *event)
{
   Command_Service *cs = data;
   Ecore_Exe_Event_Data *d = (Ecore_Exe_Event_Data *)event;

   if (cs != ecore_exe_data_get(d->exe)) return EINA_TRUE;
   DBG("cs[%p]", cs);

   eina_strbuf_append_length(cs->buf, d->data, d->size);
   return EINA_TRUE;
}

Eina_Bool
_command_del(void *data,
             int type EINA_UNUSED,
             void *event)
{
   Command_Service *cs = data;
   Ecore_Exe_Event_Del *d = event;

   if (cs != ecore_exe_data_get(d->exe)) return EINA_TRUE;
   DBG("cs[%p] d[%p] gotham[%p] shotgun[%p]",
       cs, d, cs->services->gotham, cs->services->gotham->shotgun);

   DBG("Sending answer :\n%s", eina_strbuf_string_get(cs->buf));

   DBG("cs->jid[%s] cs->services->gotham->alfred->jid[%s]",
       cs->jid, cs->services->gotham->alfred->jid);

   if (strncmp(cs->jid, cs->services->gotham->alfred->jid, strlen(cs->services->gotham->alfred->jid)))
     shotgun_message_send(cs->services->gotham->shotgun,
                          cs->jid, eina_strbuf_string_get(cs->buf),
                          SHOTGUN_MESSAGE_STATUS_ACTIVE, EINA_TRUE);
   else
     {
        Gotham_Citizen *citizen;
        citizen = eina_hash_find(cs->services->gotham->citizens, cs->jid);
        gotham_command_json_answer(".service", "", EINA_TRUE, cs->buf,
                                   cs->services->gotham, citizen, EINA_FALSE);
     }

   _command_service_free(cs);
   return EINA_TRUE;
}

Eina_Bool
_command_error(void *data,
               int type EINA_UNUSED,
               void *event)
{
   Command_Service *cs = data;
   Ecore_Exe_Event_Del *d = event;

   if (cs != ecore_exe_data_get(d->exe)) return EINA_TRUE;

   DBG("cs[%p] d[%p]", cs, d);
   ERR("Execution error");

   _command_service_free(cs);
   return EINA_TRUE;
}

#define _MSG(_a, _b, _c)                                                       \
   do {                                                                        \
      if (_a->citizen->type == GOTHAM_CITIZEN_TYPE_ALFRED)                     \
        {                                                                      \
           Eina_Strbuf *buf = eina_strbuf_new();                               \
           eina_strbuf_append(buf, _c);                                        \
           gotham_command_json_answer(".service", _b, EINA_TRUE, _c,           \
                           _a->citizen->gotham, _a->citizen);                  \
        }                                                                      \
      else gotham_command_send(_a, _b);                                        \
   } while(0)
/* "debug" */

void
command_start(Module_Services *services,
              Gotham_Citizen_Command *command)
{
   DBG("services[%p] command[%p]", services, command);
   _INVALID(command);

#ifdef _WIN32
   if (!command_win32_start(services, command, command->message + 15))
     return;

   _MSG(command, "start", "Service started");
#else
   _RUN(services, command->jid, services->conf->commands->start, command->message + 15);
#endif
}

void
command_restart(Module_Services *services,
                Gotham_Citizen_Command *command)
{
   _INVALID(command);

#ifdef _WIN32
   command_win32_stop(services, command, command->message + 17);
   if (!command_win32_start(services, command, command->message + 17))
     return;

   _MSG(command, "restart", "Service restarted");
#else
   _RUN(services, command->jid, services->conf->commands->restart, command->message + 17);
#endif
}

void
command_stop(Module_Services *services,
             Gotham_Citizen_Command *command)
{
   DBG("services[%p] command[%p]", services, command);
   _INVALID(command);

#ifdef _WIN32
   if (!command_win32_stop(services, command, command->message + 14))
     return;

   _MSG(command, "stop", "Service is stopping");
#else
   _RUN(services, command->jid, services->conf->commands->stop, command->message + 14);
#endif
}

void
command_status(Module_Services *services,
               Gotham_Citizen_Command *command)
{
   DBG("services[%p] command[%p]", services, command);
   _INVALID(command);

#ifdef _WIN32
   command_win32_status(services, command, command->message + 16);
#else
   _RUN(services, command->jid, services->conf->commands->stop, command->message + 16);
#endif
}

#undef _ANSWER
#undef _INVALID
