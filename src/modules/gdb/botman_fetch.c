#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gdb.h"

void
botman_fetch_send(Module_Gdb *gdb,
                  Gotham_Citizen_Command *command)
{
   char *coredump;
   Eina_Bool r;

   coredump = utils_dupf("%s%s", gdb->dumps.dir, command->command[2]);

   {
     struct stat b;
     int ret;

     ret = stat(coredump, &b);
     if (ret)
       {
          gotham_citizen_send(command->citizen, "Given coredump file doesnt exist");
          goto end;
       }

   }

   r = backtrace_new(gdb, coredump, command->citizen->jid, EINA_FALSE);
   if (!r)
     {
        if (strcmp(command->citizen->jid, gdb->gotham->alfred->jid))
          gotham_citizen_send(command->citizen, "Failed to fetch coredump.");
        else
          module_json_answer(".gdb", "fetch", EINA_FALSE, NULL, gdb->gotham, command->citizen, EINA_FALSE);
     }
end:
   free(coredump);
}
