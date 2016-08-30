#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <cJSON.h>
#include <ctype.h>

#include "sysinfo.h"

/**
 * @addtogroup Gotham_Module_Sysinfo
 * @{
 */

/**
 * Authorization macro
 */
#define AUTH(_a, _b, _c)                                                       \
{                                                                              \
   if ((_a->access_allowed) && (!_a->access_allowed(_b, _c)))                  \
     {                                                                         \
        ERR("%s is not autorized", _c->jid);                                   \
        Eina_Strbuf *buf = eina_strbuf_new();                                  \
        eina_strbuf_append(buf, "Access denied");                              \
        gotham_command_json_answer(".ssh", "", EINA_FALSE,                     \
                           buf, _a->gotham, _c, EINA_FALSE);                   \
        eina_strbuf_free(buf);                                                 \
        return EINA_TRUE;                                                      \
     }                                                                         \
}

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
   Module_Sysinfo *obj = data;
   obj->access_allowed = gotham_modules_function_get("access",
                                                     "access_allowed");
   return EINA_TRUE;
}

/**
 * @brief Callback when we connect.
 * If we are a botman, send our .sysinfo to Alfred
 * @param data Module object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
/*Eina_Bool
event_connect(void *data,
              int type EINA_UNUSED,
              void *ev EINA_UNUSED)
{
   Module_Sysinfo *obj = data;
   Gotham_Citizen_Command *command;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_TRUE);

   DBG("obj[%p]", obj);

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        NFO("Sending .sysinfo to Alfred");
        command = gotham_command_new(obj->gotham->alfred, ".sysinfo");
        botman_sysinfo_send(obj, command);
        gotham_command_free(command);
     }

   return EINA_TRUE;
}*/

/**
 * @brief Callback when a citizen connects.
 * If this citizen is Alfred, send him the botman's system info
 * @param data Module object
 * @param type UNUSED
 * @param ev Gotham_Citizen structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_connect(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Sysinfo *obj = data;
   Gotham_Citizen *citizen = ev;
   Gotham_Citizen_Command *command;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(citizen, EINA_TRUE);

   DBG("obj[%p], citizen[%p]=“%s” type %d",
       obj, citizen, citizen->jid, citizen->type);

   if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
       (citizen->type == GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (citizen->subscribed))
     {
        NFO("Sending .sysinfo to Alfred");
        command = gotham_command_new(obj->gotham->alfred, ".sysinfo", obj->gotham->alfred->jid);
        botman_sysinfo_send(obj, command);
        gotham_command_free(command);
     }

   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * @param data Module object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Sysinfo *obj = data;
   Gotham_Citizen_Command *command = ev;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   DBG("obj[%p] command[%p]=%s", obj, command, command->name);

   /* Alfred receives a Botman answer : no auth check */
   if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
       (!strcmp(command->name, ".sysinfo")))
     {
        command->handled = EINA_TRUE;
        alfred_botman_answer_get(obj, command);
        return EINA_TRUE;
     }

   /* Alfred receives a citizen message : auth check */
   if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (command->command[1]) &&
       (!strcmp(command->name, ".sysinfo")))
     {
        command->handled = EINA_TRUE;
        AUTH(obj, gotham_modules_command_get(".sysinfo"),
             command->citizen);
        alfred_sysinfo_show(obj, command);
     }
   /* Botman reveices a message : auth check */
   else if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
            (!command->command[1]) &&
            (!strcmp(command->name, ".sysinfo")))
     {
        command->handled = EINA_TRUE;
        AUTH(obj, gotham_modules_command_get(".sysinfo"),
             command->citizen);
        botman_sysinfo_send(obj, command);
     }
   /* Botman receives another command : search in obj->commands,
      handle it if needeed */
   else if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
            (eina_hash_find(obj->commands, command->name)))
     {
        command->handled = EINA_TRUE;
        AUTH(obj, gotham_modules_command_get(command->name),
             command->citizen);
        botman_sysinfo_command_run(obj, command);
     }

   return EINA_TRUE;

}

#undef AUTH

/**
 * @}
 */
