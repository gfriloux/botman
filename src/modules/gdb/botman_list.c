#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gdb.h"

void
botman_list_send(void *data,
                 Gotham_Citizen_Command *command)
{
   Module_Gdb *gdb = data;
   Eina_List *l;
   char *s;
   struct stat b;
   int r;
   Eina_Strbuf *buf;

   AUTH(gdb, gotham_modules_command_get(".gdb list"), command->citizen);

   buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_GOTO(buf, error);

   eina_strbuf_append(buf, "\nList of coredumps :\n");

   EINA_LIST_FOREACH(gdb->dumps.known, l, s)
     {
        DBG("s[%s]", s);

        r = stat(s, &b);
        if (r) continue;

        s = utils_coredump_report_format(s, b.st_size, b.st_mtime);
        EINA_SAFETY_ON_NULL_GOTO(s, end_loop);

        DBG("s[%s]", s);
        eina_strbuf_append_printf(buf, "\t%s\n", s);

end_loop:
        continue;
     }

   if (strcmp(command->citizen->jid, gdb->gotham->alfred->jid))
     gotham_command_send(command, eina_strbuf_string_get(buf));
   else gotham_command_json_answer(".gdb", "list", EINA_TRUE, buf, gdb->gotham, command->citizen, EINA_FALSE);

   eina_strbuf_free(buf);
   return;

error:
   if (strcmp(command->citizen->jid, gdb->gotham->alfred->jid))
     gotham_command_send(command, "Failed to list coredumps due to memory allocation error");
   else
     gotham_command_json_answer(".gdb", "list", EINA_FALSE, NULL, gdb->gotham, command->citizen, EINA_FALSE);
}
