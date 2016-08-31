#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <ctype.h>

#include "sysinfo.h"

/**
 * @addtogroup Gotham_Module_Sysinfo
 * @{
 */

/**
 * @brief Print a line for the given citizen.
 * Will show his online status, xmpp account and all declared sysinfo vars.
 * @param obj Module object
 * @param citizen Gotham_Citizen to print
 * @return const char line representing citizen
 */
const char *
_citizen_result_print(Module_Sysinfo *obj,
                      Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   Eina_Iterator *it;
   void *data;
   const char *ptr;
   Eina_Bool found = EINA_FALSE;

   buf = eina_strbuf_new();

   /* Run through obj->hw (values to found in custom vars) */
   it = eina_hash_iterator_key_new(obj->hw);

   while(eina_iterator_next(it, &data))
     {
        const char *var;
        const char *name = data;
        char item[strlen(name)+8];

        sprintf(item, "sysinfo_%s", name);
        var = VARGET(item);

        if (!var) continue;

        found = EINA_TRUE;
        eina_strbuf_append_printf(buf, "\t%s : %s\n", name, var);
     }
   eina_iterator_free(it);

   if (!found)
     eina_strbuf_append(buf, "\tNo system information found\n");

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

/**
 * @brief Show sysinfo of botmans.
 * For each citizen that match pattern, get the sysinfo values (obj->hw)
 * from custom vars and send result.
 * @param obj Module object
 * @param command Gotham_Citizen_Command containing pattern to show
 */
void
alfred_sysinfo_show(Module_Sysinfo *obj,
                    Gotham_Citizen_Command *command)
{
   Gotham_Citizen *citizen;
   Eina_Strbuf *buf,
               *result_buf;
   Eina_Bool found = EINA_FALSE;
   Eina_List *l_citizen,
             *l;

   DBG("obj[%p], command[%p]", obj, command);

   EINA_SAFETY_ON_NULL_RETURN(obj);
   EINA_SAFETY_ON_NULL_RETURN(command);

   buf = eina_strbuf_new();
   result_buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "\nContacts that matched :\n");

   l_citizen = gotham_citizen_match(obj->gotham,
                                    command->command[1],
                                    GOTHAM_CITIZEN_TYPE_BOTMAN,
                                    obj->vars);

   EINA_LIST_FOREACH(l_citizen, l, citizen)
     {
        const char *line;

        line = gotham_citizen_match_print(obj->vars, citizen, EINA_TRUE, EINA_FALSE);
        eina_strbuf_append_printf(buf, "\t%s\n", line);
        eina_strbuf_append_printf(result_buf, "%s :\n", line);
        free((char *)line);

        line = _citizen_result_print(obj, citizen);
        eina_strbuf_append_printf(result_buf, "%s\n", line);
        free((char *)line);
     }

   found = !!(eina_list_count(l_citizen));
   if (found)
     eina_strbuf_append_printf(buf, "\n%s", eina_strbuf_string_get(result_buf));

   gotham_command_send(command,
                       (found) ? eina_strbuf_string_get(buf) :
                                 "No one matches given pattern");

   eina_strbuf_free(result_buf);
   eina_strbuf_free(buf);
   eina_list_free(l_citizen);
   return;
}

/**
 * @brief Used for Alfred to get a Botman's answer (JSON).
 * When a botman sends his system information, Alfred will store
 * results into citizen's custom vars.
 * @param obj Module object
 * @param command Gotham_Citizen_Command for botman answer
 */
void
alfred_botman_answer_get(Module_Sysinfo *obj,
                         Gotham_Citizen_Command *command)
{
   Gotham_Citizen *citizen = command->citizen;
   unsigned int i;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   EINA_SAFETY_ON_NULL_RETURN(command);

   DBG("obj[%p] command[%p]=%s", obj, command, command->name);

   if (!command->command[0])
     {
        ERR("No content in botman's message");
        return;
     }

   for (i=0; command->command[i]; i++)
     {
        char **split,
             *label;

        split = eina_str_split(command->command[i], " : ", 2);
        if (!split)
          {
             ERR("Failed to split “%s”", command->command[i]);
             continue;
          }

        label = calloc(1, strlen(split[0])+9);
        sprintf(label, "sysinfo_%s", split[0]);

        if (!split[1])
          {
             gotham_citizen_var_del(citizen, label);
          }
        else
          {
             VARSET(label, "%s", split[1]);
          }
        free(label);
        free(split[0]);
        free(split);
     }

   return;
}

/**
 * @brief Add commands that can be run through Alfred.
 * @param obj Module object
 */
void
alfred_commands_add(Module_Sysinfo *obj EINA_UNUSED)
{
   gotham_modules_command_add("sysinfo", ".sysinfo",
                              "[.sysinfo pattern] - "
                              "This command allows you to get some system "
                              "informations (manufacturer, serial #, ...) "
                              "for each botman that matches pattern");
}


/**
 * @Brief Remove alfred commands.
 * @param obj Module object.
 */
void
alfred_commands_del(Module_Sysinfo *obj EINA_UNUSED)
{
   gotham_modules_command_del("sysinfo", ".sysinfo");
}


/**
 * @}
 */
