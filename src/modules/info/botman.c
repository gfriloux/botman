#include "info.h"

void
_info_botman_send(Module_Info *info,
                  Gotham_Citizen *citizen)
{
   Eina_Iterator *it;
   Eina_Strbuf *buf;
   void *data;
   Eina_Bool found = EINA_FALSE;

   buf = eina_strbuf_new();

   it = gotham_citizen_var_iterate(info->gotham->me);
   while (eina_iterator_next(it, &data))
     {
        Eina_Hash_Tuple *t = data;
        const char *name = t->key,
                   *value = t->data;

        found = EINA_TRUE;
DBG("Adding %s : %s", name, value);
        eina_strbuf_append_printf(buf, "%s : %s\n", name, value);
     }
   eina_iterator_free(it);

   if (!found)
     {
        eina_strbuf_free(buf);
        return;
     }

   module_json_answer(".info", "", EINA_TRUE, buf, info->gotham, citizen, EINA_TRUE);
}

Eina_Bool
info_botman_sync(void *data)
{
   Module_Info *info = data;

   _info_botman_send(info, info->gotham->alfred);
   return EINA_TRUE;
}

void
info_botman_command(Module_Info *info,
                    Gotham_Citizen_Command *command)
{

   if (   (command->command[1])
       && (!strcmp(command->command[1], "get"))
       && (command->command[2])
      )
     {
        Eina_Strbuf *buf;
        const char *s;

        s = gotham_citizen_var_get(info->gotham->me, command->command[2]);
        EINA_SAFETY_ON_NULL_GOTO(s, not_found_msg);

        buf = eina_strbuf_new();
        if (!buf)
          goto memory_error;

        eina_strbuf_append_printf(buf, "%s : %s\n", command->command[2], s);
        if (strcmp(command->citizen->jid, info->gotham->alfred->jid))
          gotham_citizen_send(command->citizen, eina_strbuf_string_get(buf));
        else
          module_json_answer(".info", "get", EINA_TRUE, buf, info->gotham, command->citizen, EINA_TRUE);
        eina_strbuf_free(buf);
     }
   else if (!command->command[1])
     _info_botman_send(info, command->citizen);

   return;

not_found_msg:
   if (strcmp(command->citizen->jid, info->gotham->alfred->jid))
     gotham_citizen_send(command->citizen, "Given info variable not found.");
   else
     {
        Eina_Strbuf *buf;

        buf = eina_strbuf_new();
        eina_strbuf_append_printf(buf, "Given info variable '%s' was not found.", command->command[1]);

        module_json_answer(".info", "get", EINA_FALSE, buf, info->gotham, command->citizen, EINA_TRUE);
        eina_strbuf_free(buf);
     }
   return;

memory_error:
   return;
}
