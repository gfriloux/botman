#include "info.h"

void
_info_alfred_command_botman(Module_Info *info,
                            Gotham_Citizen_Command *command)
{
   Gotham_Citizen *citizen = command->citizen;
   unsigned int i = 0;


   DBG("info[%p] command[%p]", info, command);

   for (i = 0; command->command[i]; i++)
     {
        char **split;

        split = eina_str_split(command->command[i], " : ", 2);
        EINA_SAFETY_ON_NULL_GOTO(split, end_loop);

        if ((!split[1]) || (split[1][0]==0))
          {
             DBG("Removing custom var %s", split[0]);
             gotham_citizen_var_del(citizen, split[0]);
             goto free_split;
          }

        DBG("Setting custom variable “%s”=“%s”", split[0], split[1]);
        VARSET(split[0], "%s", split[1]);

free_split:
        free(split[0]);
        free(split);
end_loop:
        continue;
     }
}

const char *
_citizen_match_print(Module_Info *info,
                     Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   Eina_Array_Iterator it;
   unsigned int i;
   const char *item,
              *ptr;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "%s %s ",
                             citizen->status == GOTHAM_CITIZEN_STATUS_OFFLINE ?
                                                "offline" : "online",
                             citizen->jid);

   EINA_ARRAY_ITER_NEXT(info->search_vars, i, item, it)
     {
        const char *var = VARGET(item);

        if (var) eina_strbuf_append_printf(buf, "%s[%s] ", item, var);
     }

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

const char *
_citizen_result_print(Module_Info *info EINA_UNUSED,
                      Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   const char *ptr;
   Eina_Iterator *it;
   void *data;
   Eina_Bool found;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "%s %s\n",
                             (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE) ?
                                "offline" : "online",
                             citizen->jid);

   it = gotham_citizen_var_iterate(citizen);
   while (eina_iterator_next(it, &data))
     {
        Eina_Hash_Tuple *t = data;
        const char *name = t->key,
                   *value = t->data;

        found = EINA_TRUE;

        eina_strbuf_append_printf(buf, "\t%s : %s\n",
                                  name, value ? value : "");
     }
   eina_iterator_free(it);

   if (!found) eina_strbuf_append_printf(buf, "\tNo informations found\n");

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

void
_info_alfred_command_citizen(Module_Info *info,
                             Gotham_Citizen_Command *command)
{
   Gotham_Citizen *citizen;
   Eina_Strbuf *buf,
               *result_buf;
   Eina_Bool found = EINA_FALSE;
   Eina_List *l_citizen,
             *l;

   DBG("info[%p] command[%p]", info, command);

   if (!command->command[1])
     {
        gotham_citizen_send(command->citizen, "Usage : .info pattern");
        return;
     }

   buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_RETURN(buf);

   result_buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_GOTO(result_buf, free_buf);

   eina_strbuf_append_printf(buf, "\nContacts that matched :\n");

   l_citizen = gotham_citizen_match(info->gotham,
                                    command->command[1],
                                    GOTHAM_CITIZEN_TYPE_BOTMAN,
                                    info->search_vars);

   EINA_LIST_FOREACH(l_citizen, l, citizen)
     {
        const char *line;

        found = EINA_TRUE;

        line = _citizen_match_print(info, citizen);
        eina_strbuf_append_printf(buf, "\t%s\n", line);
        free((char *)line);

        line = _citizen_result_print(info, citizen);
        eina_strbuf_append_printf(result_buf, "%s\n", line);
        free((char *)line);
     }

   if (found)
     eina_strbuf_append_printf(buf, "\n%s", eina_strbuf_string_get(result_buf));

   gotham_citizen_send(command->citizen,
                       found ? eina_strbuf_string_get(buf) :
                               "No one matches given pattern");

   eina_strbuf_free(result_buf);
   eina_strbuf_free(buf);
   eina_list_free(l_citizen);
   return;

free_buf:
   eina_strbuf_free(buf);
}

void
info_alfred_command(Module_Info *info,
                    Gotham_Citizen_Command *command)
{
   DBG("info[%p] command[%p]", info, command);

   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     _info_alfred_command_botman(info, command);
   else
     _info_alfred_command_citizen(info, command);
}

void
info_alfred_commands_register(void)
{
   gotham_modules_command_add("info", ".info",
                              "[.info pattern] - "
                              "This command allows to access some custom "
                              "variables from botmans of a given pattern.");
}

void
info_alfred_commands_unregister(void)
{
   gotham_modules_command_del("info", ".info");
}