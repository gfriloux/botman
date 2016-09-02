#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fnmatch.h>

#include "gdb.h"

void
botman_delete_send(void *data,
                   Gotham_Citizen_Command *command)
{
   Module_Gdb *gdb = data;
   Eina_List *l;
   char *s,
        *p;
   int r;
   Eina_Strbuf *buf;

   buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_GOTO(buf, error);

   eina_strbuf_append(buf, "\nDeleted coredumps :\n");

   EINA_LIST_FOREACH(gdb->dumps.known, l, s)
     {
        DBG("s[%s]", s);

        p = strrchr(s, '/');
        EINA_SAFETY_ON_NULL_GOTO(p, end_loop);

        p = p + 1;

        DBG("Compare [%s] - [%s]", command->command[2], p);
        r = fnmatch(command->command[2], p, FNM_NOESCAPE);
        if (!r)
          {
             eina_strbuf_append_printf(buf, "\t%s\n", s);
             remove(s);
          }

end_loop:
        continue;
     }

   if (strcmp(command->citizen->jid, gdb->gotham->alfred->jid))
     gotham_command_send(command, eina_strbuf_string_get(buf));
   else gotham_command_json_answer(".gdb", "list", EINA_TRUE, buf, gdb->gotham, command->citizen, EINA_FALSE);

   eina_strbuf_free(buf);
   botman_dumps_poll(gdb);
   return;

error:
   if (strcmp(command->citizen->jid, gdb->gotham->alfred->jid))
     gotham_command_send(command, "Failed to delete coredumps due to memory allocation error");
   else
     gotham_command_json_answer(".gdb", "list", EINA_FALSE, NULL, gdb->gotham, command->citizen, EINA_FALSE);
}
