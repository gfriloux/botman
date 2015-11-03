#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <ctype.h>

#include "version.h"

/**
 * @addtogroup Gotham_Module_Version
 * @{
 */

/**
 * @brief Print a line for the given citizen.
 * Will show his online status, xmpp account and some custom vars
 * @param seen Module object
 * @param citizen Gotham_Citizen to print
 * @return const char line representing citizen
 */
const char *
version_citizen_match_print(Module_Version *version,
                             Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   Eina_Array_Iterator it;
   unsigned int i;
   const char *item,
              *ptr;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "%s %s ",
                             (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE) ?
                                "offline" : "online",
                             citizen->jid);

   EINA_ARRAY_ITER_NEXT(version->vars, i, item, it)
     {
        const char *var = VARGET(item);

        if (!var)
          continue;

        eina_strbuf_append_printf(buf, "%s[%s] ", item, var);
     }

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

/**
 * @brief Print a line for the given citizen.
 * Will show his online status, xmpp account and all decalred version vars.
 * @param seen Module object
 * @param citizen Gotham_Citizen to print
 * @return const char line representing citizen
 */
const char *
_version_citizen_result_print(Module_Version *version EINA_UNUSED,
                              Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   const char *ptr;
   Eina_Iterator *it;
   void *data;

   buf = eina_strbuf_new();

   it = eina_hash_iterator_tuple_new(citizen->vars);
   while (eina_iterator_next(it, &data))
     {
        Eina_Hash_Tuple *t = data;
        const char *k = t->key,
                   *v = t->data;


        if (strncmp(k, "version_", 8))
          continue;

        eina_strbuf_append_printf(buf, "\t%s : %s\n", k + 8, v);
     }

   if (!eina_strbuf_length_get(buf))
     eina_strbuf_append(buf, "\tNo version information found\n");

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

/**
 * @brief Get version info for accounts matching pattern.
 * Alfred searches in his custom vars and replies with that.
 * @param version Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
version_alfred_command(Module_Version *version,
                       Gotham_Citizen_Command *command)
{
   Gotham *gotham;
   Gotham_Citizen *citizen;
   Eina_Strbuf *buf,
               *result_buf;
   const char **version_cmd = command->command;
   Eina_Bool found = EINA_FALSE;
   Eina_List *l_citizen,
             *l;

   EINA_SAFETY_ON_NULL_RETURN(version);
   EINA_SAFETY_ON_NULL_RETURN(command);

   gotham = version->gotham;

   if (!version_cmd[1])
     {
        gotham_command_send(command, "Usage : .version pattern");
        return;
     }

   buf = eina_strbuf_new();
   result_buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "\nContacts that matched :\n");

   l_citizen = gotham_citizen_match(gotham,
                                    version_cmd[1],
                                    GOTHAM_CITIZEN_TYPE_BOTMAN,
                                    version->vars);

   EINA_LIST_FOREACH(l_citizen, l, citizen)
     {
        const char *line;

        found = EINA_TRUE;

        line = version_citizen_match_print(version, citizen);
        eina_strbuf_append_printf(buf, "\t%s\n", line);
        eina_strbuf_append_printf(result_buf, "%s :\n", line);
        free((char *)line);

        line = _version_citizen_result_print(version, citizen);
        eina_strbuf_append_printf(result_buf, "%s\n", line);
        free((char *)line);
     }

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
 * @brief Treat a Botman's answer for a .version request.
 * When a botman sends his version information, Alfred will store
 * results into citizen's custom vars.
 * @param version Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
version_alfred_answer_get(Module_Version *version,
                          Gotham_Citizen_Command *command)
{
   Gotham_Citizen *citizen;
   const char **version_cmd;
   unsigned int i=0;

   EINA_SAFETY_ON_NULL_RETURN(version);
   EINA_SAFETY_ON_NULL_RETURN(command);

   DBG("version[%p] command[%p]=%s", version, command, command->name);

   citizen = command->citizen;
   version_cmd = command->command;

   for (i=0; version_cmd[i]; i++)
     {
        char **split,
             *label,
             *p;

        split = eina_str_split(version_cmd[i], " : ", 2);
        if (!split)
          {
             ERR("Failed to split “%s”", version_cmd[i]);
             continue;
          }
        label = calloc(1, strlen(split[0])+9);
        sprintf(label, "version_%s", split[0]);
        p=label;

        for (; *p; ++p)
          *p = tolower(*p);

        if (!split[1])
          gotham_citizen_var_del(citizen, label);
        else
          VARSET(label, "%s", split[1]);
        free(label);
        free(split[0]);
        free(split);
     }

   return;
}

/**
 * @brief Register commands that Alfred can handle.
 * Command list :<br />
 * .version pattern : return software versions for each account
                      matching pattern
 * .version find software sign version : List accounts that match
                                         the search pattern.
 */
void
version_alfred_commands_register(void)
{
   gotham_modules_command_add("version", ".version",
                              "[.version pattern] - "
                              "This command returns the version of some "
                              "installed softwares. Software list and custom "
                              "version commands can be specified in "
                              "version.conf.");

   gotham_modules_command_add("version", ".version find",
                              "[.version find software sign version] - "
                              "This command will list all botmans that "
                              "match the [software_name] [sign] [version].\n"
                              "[sign] can be : <, <=, =, >=, >");
}

/**
 * @brief Unregister Alfred commands.
 */
void
version_alfred_commands_unregister(void)
{
   gotham_modules_command_del("version", ".version");
   gotham_modules_command_del("version", ".version find");
}

/**
 * @}
 */
