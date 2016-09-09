#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <ctype.h>

#include "sysinfo.h"

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

#undef AUTH

/**
 * @}
 */
