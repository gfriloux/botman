#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <fnmatch.h>

#include "seen.h"

/**
 * @addtogroup Gotham_Module_Seen
 * @{
 */

/**
 * @brief Print a line for the given citizen.
 * Will show his online status, xmpp account and some custom vars
 * @param seen Module_Seen object
 * @param citizen Gotham_Citizen to print
 * @return const char line representing citizen
 */
const char *
_citizen_print(Module_Seen *seen,
               Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   Eina_Array_Iterator it;
   unsigned int i;
   const char *item,
              *ptr,
              *last_time;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "\t%s %s ",
                             (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE) ?
                                "offline" : "online",
                             citizen->jid);

   EINA_ARRAY_ITER_NEXT(seen->vars, i, item, it)
     {
        const char *var = VARGET(item);

        if (!var)
          continue;

        eina_strbuf_append_printf(buf, "%s[%s] ", item, var);
     }

   if (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE)
     {
        double timestamp;
        const char *seen_last = NULL;

        seen_last = VARGET("seen_last");

        timestamp = time(0) - ((seen_last) ? atof(seen_last) : 0);
        last_time = seen_utils_elapsed_time(timestamp);
        eina_strbuf_append_printf(buf, "time %s", last_time);
        free((char *)last_time);
     }

   eina_strbuf_append(buf, "\n");

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module_Seen object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Seen *seen = data;
   seen->access_allowed = gotham_modules_function_get("access",
                                                      "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * Then search citizens that match pattern and send back citizen list.
 * @param data Module_Module object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Seen *seen = data;
   Gotham *gotham;
   Gotham_Citizen *citizen;
   Gotham_Citizen_Command *command = ev;
   Eina_Strbuf *buf;
   const char **seen_cmd = command->command;
   Eina_Bool found = EINA_FALSE;
   Eina_List *l_citizen,
             *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(seen, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   DBG("seen[%p] command[%p]=%s", seen, command, command->name);

   gotham = seen->gotham;

   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if (strcmp(command->name, ".seen"))
     return EINA_TRUE;

   command->handled = EINA_TRUE;

   if ((seen->access_allowed) &&
       (!seen->access_allowed(gotham_modules_command_get(".seen"),
                              command->citizen)))
     {
        gotham_citizen_send(command->citizen, "Access denied");
        return EINA_TRUE;
     }

   if (!seen_cmd[1])
     {
        gotham_citizen_send(command->citizen, "Usage : .seen pattern");
        return EINA_TRUE;
     }

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "\nContacts that matched :\n");

   l_citizen = gotham_citizen_match(gotham,
                                    seen_cmd[1],
                                    GOTHAM_CITIZEN_TYPE_BOTMAN,
                                    seen->vars);

   EINA_LIST_FOREACH(l_citizen, l, citizen)
     {
        const char *line;

        found = EINA_TRUE;
        line = _citizen_print(seen, citizen);
        eina_strbuf_append(buf, line);
        free((char *)line);
     }

   gotham_citizen_send(command->citizen,
                       (found) ? eina_strbuf_string_get(buf) :
                                 "No one matches given pattern");

   eina_strbuf_free(buf);
   eina_list_free(l_citizen);
   return EINA_TRUE;
}

/**
 * @brief Callback for a citizen disconnection.
 * Update the seen_last custom var with disconnect time.
 * @param data Module_Seen object
 * @param type UNUSED
 * @param ev Gotham_Citizen that has disconnected
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_disconnect(void *data,
                         int type EINA_UNUSED,
                         void *ev)
{
   Module_Seen *seen = data;
   Gotham_Citizen *citizen = ev;

   DBG("seen[%p] citizen[%p] time=%.0f",
       seen, citizen, (double)time(0));


   VARSET("seen_last", "%.0f", (double)time(0));

   return EINA_TRUE;
}

/**
 * @}
 */
