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
 * @}
 */
