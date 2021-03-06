#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "spam.h"

/**
 * @addtogroup Gotham_Module_Spam
 * @{
 */

/**
 * Authorization macro
 */
#define AUTH(_a, _b, _c)                                                       \
   {                                                                           \
      if ((_a) && (!_a(gotham_modules_command_get(_b), _c->citizen)))          \
        {                                                                      \
           gotham_command_send(_c, "Access denied");                           \
           return EINA_TRUE;                                                   \
        }                                                                      \
   }

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module_Spam object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Spam *spam = data;

   spam->access_allowed = gotham_modules_function_get("access",
                                                      "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * First, list contacts that match and reply this list to sender.
 * Then send command to each citizen that match
 * @param data Module_Spam object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Spam *spam = data;
   Gotham_Citizen *bot;
   Gotham_Citizen_Command *command = ev;
   const char **spam_cmd,
              *m;
   Eina_List *lc,
             *l;
   Eina_Strbuf *buf;

   EINA_SAFETY_ON_NULL_RETURN_VAL(spam, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   spam_cmd = command->command;

   if ((command->citizen->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) ||
       (strcmp(command->name, ".spam")))
     {
        queue_check(spam, command);
        return EINA_TRUE;
     }

   command->handled = EINA_TRUE;

   AUTH(spam->access_allowed, ".spam", command);
   GOTHAM_IF_SEND_RETURN_VAL((!spam_cmd[1] || !spam_cmd[2]), command,
                             "Wrong arguments. Usage : .spam <pattern> <command>", EINA_TRUE);

   lc = gotham_citizen_match(spam->gotham,
                             spam_cmd[1],
                             GOTHAM_CITIZEN_TYPE_BOTMAN,
                             spam->conf->vars);
   GOTHAM_IF_SEND_RETURN_VAL(!lc, command, "No one matches your query", EINA_TRUE);

   /**
    * List contacts that match and reply to sender with this list
    * before sending commands to botmans.
    * Contact list and command send are made in two separate EINA_LIST_FOREACH
    * to prevent a botmans result to be sent back before the contact list
    */
   buf = eina_strbuf_new();
   eina_strbuf_append(buf, "\nContacts that matched :\n");
   EINA_LIST_FOREACH(lc, l, bot)
     {
        const char *line;
        line = gotham_citizen_match_print(spam->conf->vars, bot, EINA_TRUE, EINA_FALSE);
        eina_strbuf_append_printf(buf, "\t%s\n", line);
        free((char *)line);
     }
   gotham_command_send(command, eina_strbuf_string_get(buf));
   eina_strbuf_free(buf);

   m = utils_atos(&spam_cmd[2], " ");
   EINA_LIST_FREE(lc, bot)
     {
        gotham_citizen_send(bot, m);
        queue_add(spam, command->citizen, bot, command->command[2], command->jid);
     }
   free((char *)m);

   return EINA_TRUE;
}

#undef AUTH

/**
 * @}
 */
