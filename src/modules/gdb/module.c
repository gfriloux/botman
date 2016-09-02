#include "gdb.h"

static const char *name = "GDB";
int _gdb_log_dom = -1;

const char *
module_init(void)
{
   eina_init();
   _gdb_log_dom = eina_log_domain_register("module_gdb", EINA_COLOR_RED);
   ecore_init();

   DBG("void");

   return name;
}

void
module_shutdown(void)
{
   DBG("void");

   ecore_shutdown();
   eina_log_domain_unregister(_gdb_log_dom);
   eina_shutdown();
}

void *
module_register(Gotham *gotham)
{
   Module_Gdb *gdb;

   if (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN) return NULL;

   gdb = calloc(1, sizeof(Module_Gdb));
   EINA_SAFETY_ON_NULL_RETURN_VAL(gdb, NULL);
   gdb->gotham = gotham;
   gdb->conf = gotham_serialize_file_to_struct(MODULE_GDB_CONF,  (Gotham_Deserialization_Function)azy_value_to_Module_Gdb_Conf);
   gdb->dumps.known = gotham_serialize_file_to_struct(MODULE_GDB_SAVE, (Gotham_Deserialization_Function)azy_value_to_Array_string);

   DBG("gdb->conf->notification[%p][%s] gdb->conf->dir[%p][%s]",
       gdb->conf->notification, gdb->conf->notification, gdb->conf->dir, gdb->conf->dir);

   gdb->dumps.poll = ecore_timer_add(20.0, botman_dumps_poll, gdb);

   gotham_modules_command_add("gdb", ".gdb list",
                              "[.gdb list] - "
                              "This command will list coredumps inside the coredumps directory.",
                              botman_list_send);
   gotham_modules_command_add("gdb", ".gdb delete",
                              "[.gdb delete <pattern>] - "
                              "This command will delete coredumps matching given parttern.",
                              botman_delete_send);
   gotham_modules_command_add("gdb", ".gdb fetch",
                              "[.gdb fetch <coredump>] - "
                              "This command will retrieve the backtrace from a given coredump.",
                              botman_fetch_send);

   return gdb;
}

void
module_unregister(void *data)
{
   Module_Gdb *gdb = data;
   Eina_List *l;
   char *s;

   EINA_LIST_FOREACH(gdb->dumps.known, l, s)
     free(s);

   ecore_timer_del(gdb->dumps.poll);
}
