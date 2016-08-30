#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <cJSON.h>
#include <ctype.h>

#include "ssh_tunnel.h"

/**
 * @addtogroup Gotham_Module_Ssh_Tunnel
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
/* "debug" */

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allowed. Also set the pointer
 * for the conf save function.
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
   Module_Ssh_Tunnel *obj = data;
   obj->access_allowed = gotham_modules_function_get("access",
                                                     "access_allowed");
   obj->save_conf = gotham_modules_function_get("save",
                                                "save_conf");
   return EINA_TRUE;
}

/**
 * @brief Callback when connected.
 * Gather tunnel informations (pid and port) from custom vars and check
 * if it's still valid using ssh_tunnel_check.
 * Also gather licence information from custom vars.
 * @param data Module object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_connect(void *data,
              int type EINA_UNUSED,
              void *ev EINA_UNUSED)
{
   Module_Ssh_Tunnel *obj = data;
   Gotham_Citizen *citizen = obj->gotham->me;
   const char *pid,
              *port;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_TRUE);

   DBG("obj[%p]", obj);

   if (obj->gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   /* Get tunnel informations and check their validity */
   pid = VARGET("tunnel_pid");
   obj->tunnel.pid = (pid ? atoi(pid) : 0);
   port = VARGET("tunnel_port");
   obj->tunnel.port = (port ? atoi(port) : 0);

   ssh_tunnel_check(obj);

   return EINA_TRUE;
}


/**
 * @brief Callback when a citizen connects.
 * If this citizen is Alfred, send him the current tunnel status
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
   Module_Ssh_Tunnel *obj = data;
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
        NFO("Sending .ssh to Alfred");
        command = gotham_command_new(citizen, ".ssh", citizen->jid);
        ssh_tunnel_get(obj, command);
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
   Module_Ssh_Tunnel *obj = data;
   Gotham_Citizen_Command *command = ev;

   EINA_SAFETY_ON_NULL_RETURN_VAL(obj, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   if (strcmp(command->name, ".ssh"))
     return EINA_TRUE;

   DBG("obj[%p] command[%p]=%s", obj, command, command->name);

   /* Alfred receives a Botman answer : no auth check */
   if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN))
     {
        command->handled = EINA_TRUE;
        botman_answer_get(obj, command);
        return EINA_TRUE;
     }

   /* Alfred receives a citizen message : auth check */
   if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (!command->command[1]))
     {
        command->handled = EINA_TRUE;
        AUTH(obj, gotham_modules_command_get(".ssh"),
             command->citizen);
        alfred_command_tunnels_show(obj, command);
     }
   /* Botman reveices a message : auth check */
   else if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
            (command->command[1]) &&
            (!strcmp(command->command[1], "on")))
     {
        command->handled = EINA_TRUE;
        AUTH(obj, gotham_modules_command_get(".ssh on"),
             command->citizen);
        ssh_tunnel_open(obj, command);
     }
   else if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
            (command->command[1]) &&
            (!strcmp(command->command[1], "off")))

     {
        command->handled = EINA_TRUE;
        AUTH(obj, gotham_modules_command_get(".ssh off"),
             command->citizen);
        ssh_tunnel_close(obj, command);
     }
   else if ((obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
            (!command->command[1]))
     {
        command->handled = EINA_TRUE;
        AUTH(obj, gotham_modules_command_get(".ssh"),
             command->citizen);
        ssh_tunnel_get(obj, command);
     }

   return EINA_TRUE;
}

#undef AUTH

/**
 * @}
 */
