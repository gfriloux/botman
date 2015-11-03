#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <fnmatch.h>

#include "rewrite.h"

/**
 * @addtogroup Gotham_Module_Rewrite
 * @{
 */

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Rewrite *obj = data;

   obj->access_allowed = gotham_modules_function_get("access",
                                                     "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief List existing rewrite rules.
 * @param rewrite Module_Rewrite object
 * @return const char * list of rewrited functions
 */
const char *
_event_citizen_command_list(Module_Rewrite *rewrite)
{
   Eina_Strbuf *b;
   const char *s;
   Module_Rewrite_Rule *rule;

   b = eina_strbuf_new();
   eina_strbuf_append(b, "\nList of rewrites rules :\n");

   EINA_INLIST_FOREACH(rewrite->rules, rule)
     {
        eina_strbuf_append_printf(b,
                                  "\tRule [%s]\n"
                                  "\t\tFilter : %s\n"
                                  "\t\tRewrite command : %s\n"
                                  "\t\tDescription : %s\n",
                                  rule->name, rule->filter,
                                  rule->rule, rule->desc);
     }

   s = eina_strbuf_string_steal(b);
   eina_strbuf_free(b);
   return s;
}

/**
 * @brief Callback when a citizen sends a command.
 * @warning No Auth Check here, auth check will be made on rewrited command.
 * @warning Problem is that new (rewrited) command is thrown with Alfred's account,
 * @warning we don't know who launched the original command and what his auth rights were.
 * Search in rules if one matches current command. If so, generate
 * a new command and send it (gotham_event_command_new)
 * @param data Module_Rewrite object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Rewrite *rewrite = data;
   Module_Rewrite_Rule *rule;
   Gotham_Citizen_Command *command = ev,
                          *gcc;
   const char *s;

   DBG("rewrite[%p] command[%p]=%s", rewrite, command, command->name);

   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if (!strcmp(command->name, ".rewrite"))
     {
        command->handled = EINA_TRUE;
        const char *answer = _event_citizen_command_list(rewrite);
        gotham_command_send(command,
                            (answer) ? answer : "Failed to execute command");
        free((char *)answer);
        return EINA_TRUE;
     }

   gcc = eina_hash_find(rewrite->rw, command);
   if (gcc)
     {
        eina_hash_del(rewrite->rw, gcc, NULL);
        return EINA_TRUE;
     }

   EINA_INLIST_FOREACH(rewrite->rules, rule)
     {
        if (fnmatch(rule->filter, command->message, FNM_NOESCAPE))
          continue;

        if ((rewrite->access_allowed) &&
           (!rewrite->access_allowed(gotham_modules_command_get(rule->name),
                                     command->citizen)))
          {
             command->handled = EINA_TRUE;
             gotham_command_send(command, "Access denied");
             return EINA_TRUE;
          }

        /* We have one command to rewrite! */
        s = utils_rewrite(rule, command->message);
        if (!s)
          break;

        DBG("New command : %s", s);
        command->handled = EINA_TRUE;

        gcc = gotham_command_new(command->citizen, s, command->jid);
        gotham_event_command_new(gcc);
        eina_hash_add(rewrite->rw, gcc, gcc);

        free((char *)s);
        break;
     }
   return EINA_TRUE;
}

/**
 * @}
 */
