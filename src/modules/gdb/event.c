#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gdb.h"

#define AUTH(_a, _b, _c)                                                       \
{                                                                              \
   if ((_a->access_allowed) && (!_a->access_allowed(_b, _c)))                  \
     {                                                                         \
        ERR("%s is not autorized", _c->jid);                                   \
        Eina_Strbuf *buf = eina_strbuf_new();                                  \
        eina_strbuf_append(buf, "Access denied");                              \
        module_json_answer(".gdb", "", EINA_FALSE,                             \
                           buf, _a->gotham, _c, EINA_FALSE);                   \
        eina_strbuf_free(buf);                                                 \
        return EINA_TRUE;                                                      \
     }                                                                         \
}
/* "debug" */

Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Gdb *gdb = data;
   Gotham_Citizen_Command *command = ev;

   EINA_SAFETY_ON_NULL_RETURN_VAL(gdb, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   if (strcmp(command->name, ".gdb"))
     return EINA_TRUE;

   DBG("gdb[%p] command[%p][%s]", gdb, command, command->name);

   if (gdb->gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if (strncmp(command->name, ".gdb", 4))
     return EINA_TRUE;

   command->handled = EINA_TRUE;

   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        if (!command->command[1])
          {
             AUTH(gdb, gotham_modules_command_get(".gdb"), command->citizen);
             alfred_command_list(gdb, command);
          }
        /**
        * In this case, Botman is sending his backtrace to Alfred
        * We should forward it to the nightwatchers
        */
        else if ((command->parameters) && (!strcmp(command->parameters, "backtrace")))
          alfred_backtrace_forward(gdb, command);
        else if ((command->parameters) && (!strcmp(command->parameters, "add")))
          {
             AUTH(gdb, gotham_modules_command_get(".gdb add"), command->citizen);
             alfred_command_add(gdb, command);
          }
        else if ((command->parameters) && (!strcmp(command->parameters, "del")))
          {
             AUTH(gdb, gotham_modules_command_get(".gdb del"), command->citizen);
             alfred_command_del(gdb, command);
          }

        return EINA_TRUE;
     }

   if (!strcmp(command->command[1], "list"))
     {
        AUTH(gdb, gotham_modules_command_get(".gdb list"), command->citizen);
        botman_list_send(gdb, command);
     }
   else if (!strcmp(command->command[1], "delete"))
     {
        if (command->command[2])
          {
             AUTH(gdb, gotham_modules_command_get(".gdb delete"), command->citizen);
             botman_delete_send(gdb, command);
          }
        else gotham_citizen_send(command->citizen, "Usage : .gdb delete coredump");
     }
   else if (!strcmp(command->command[1], "fetch"))
     {
        if (command->command[2])
          {
             AUTH(gdb, gotham_modules_command_get(".gdb fetch"), command->citizen);
             botman_fetch_send(gdb, command);
          }
        else gotham_citizen_send(command->citizen, "Usage : .gdb fetch coredump");
     }
   return EINA_TRUE;
}
