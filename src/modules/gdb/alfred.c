#include "gdb.h"


void
alfred_command_list(Module_Gdb *gdb,
                    Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;
   const char *p,
              *heroe;
   Eina_Array_Iterator iterator;
   unsigned int i;

   buf = eina_strbuf_new();
   eina_strbuf_append(buf, "\nList of heroes :\n");

   EINA_ARRAY_ITER_NEXT(gdb->heroes, i, heroe, iterator)
     eina_strbuf_append_printf(buf, "\t%s\n", heroe);

   p = eina_strbuf_string_get(buf);
   gotham_citizen_send(command->citizen, p);
   eina_strbuf_free(buf);
}

void
alfred_command_add(Module_Gdb *gdb,
                   Gotham_Citizen_Command *command)
{
   char *heroe;
   Eina_Array_Iterator iterator;
   unsigned int i;
   const char *jid = NULL;
   Gotham_Citizen *citizen;

   if (!command->command[2]) return;

   jid = command->command[2];
   EINA_ARRAY_ITER_NEXT(gdb->heroes, i, heroe, iterator)
     if (!strcmp(heroe, jid)) goto exists;

   citizen = eina_hash_find(gdb->gotham->citizens, jid);
   EINA_SAFETY_ON_NULL_GOTO(citizen, not_found);

   eina_array_push(gdb->heroes, eina_stringshare_add(jid));
   conf_save(gdb);
   gotham_citizen_send(command->citizen, "A new nightwatcher has arise");
   return;

not_found:
   gotham_citizen_send(command->citizen, "Unknown heroe specified");
   return;
exists:
   gotham_citizen_send(command->citizen, "This heroe is already a nightwatcher!");
}

Eina_Bool
_alfred_command_del_keep(void *data, void *gdata)
{
   const char *heroe_fallen = gdata,
              *heroe_found = data;

   if (strcmp(heroe_found, heroe_fallen)) return EINA_TRUE;

   eina_stringshare_del(heroe_found);
   return EINA_FALSE;
}

void
alfred_command_del(Module_Gdb *gdb,
                   Gotham_Citizen_Command *command)
{
   DBG("gdb[%p] command[%p]", gdb, command);

   if (!command->command[2]) return;

   eina_array_remove(gdb->heroes, _alfred_command_del_keep,
                     (void *)command->command[2]);
   conf_save(gdb);

   gotham_citizen_send(command->citizen, "Rest in pieces");
}


void
_alfred_backtrace_send(Module_Gdb *gdb,
                       const char *backtrace)
{
   Gotham *gotham;
   const char *heroe;
   Eina_Array_Iterator iterator;
   unsigned int i;
   Gotham_Citizen *citizen;

   EINA_SAFETY_ON_NULL_RETURN(gdb);
   EINA_SAFETY_ON_NULL_RETURN(backtrace);

   gotham = gdb->gotham;

   EINA_ARRAY_ITER_NEXT(gdb->heroes, i, heroe, iterator)
     {
        citizen = eina_hash_find(gotham->citizens, heroe);
        if (!citizen)
          continue;

        gotham_citizen_send(citizen, backtrace);
     }
   return;
}

void
alfred_backtrace_forward(Module_Gdb *gdb,
                         Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;
   Gotham_Citizen *citizen;

   EINA_SAFETY_ON_NULL_RETURN(gdb);
   EINA_SAFETY_ON_NULL_RETURN(command);

   DBG("gdb[%p] command[%p]", gdb, command);

   citizen = command->citizen;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf,
                             "Received backtrace from %s ",
                             citizen->jid);

   if (citizen->nickname)
     eina_strbuf_append_printf(buf, "(%s) ", citizen->nickname);

   eina_strbuf_append(buf, ":\n");
   eina_strbuf_append(buf, command->command[0]);
   _alfred_backtrace_send(gdb, eina_strbuf_string_get(buf));
   eina_strbuf_free(buf);
   return;
}

void
alfred_register(Module_Gdb *gdb)
{
   conf_load(gdb);

   gotham_modules_command_add("gdb", ".gdb",
                              "[.gdb] - "
                              "This command will list heroes that get "
                              "called when we find a crime scene.");
   gotham_modules_command_add("gdb", ".gdb add",
                              "[.gdb add jid] - "
                              "This command will raise a citizen to the "
                              "level of an heroe, by joining the "
                              "nightwatchers.");
   gotham_modules_command_add("gdb", ".gdb del",
                              "[.gdb del jid] - "
                              "This command will remove a fallen heroe.");
}
