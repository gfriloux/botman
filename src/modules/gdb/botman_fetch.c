#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gdb.h"

void
botman_fetch_send(void *data,
                  Gotham_Citizen_Command *command)
{
   Module_Gdb *gdb = data;
   char *coredump;
   Eina_Bool r;

   AUTH(gdb, gotham_modules_command_get(".gdb fetch"), command->citizen);

   coredump = utils_dupf("%s%s", gdb->conf->dir, command->command[2]);

   {
     struct stat b;
     int ret;

     ret = stat(coredump, &b);
     if (ret)
       {
          gotham_command_send(command, "Given coredump file doesnt exist");
          goto end;
       }

   }

   r = backtrace_new(gdb, coredump, command->citizen->jid, EINA_FALSE);
   if (!r)
     {
        if (strcmp(command->citizen->jid, gdb->gotham->alfred->jid))
          gotham_command_send(command, "Failed to fetch coredump.");
        else
          gotham_command_json_answer(".gdb", "fetch", EINA_FALSE, NULL, gdb->gotham, command->citizen, EINA_FALSE);
     }
end:
   free(coredump);
}
