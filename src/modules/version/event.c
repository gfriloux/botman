#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

#include "version.h"

/**
 * @brief Callback for a citizen connection.
 * @param data Module object
 * @param type UNUSED
 * @param ev Gotham_Citizen that has disconnected
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_connect(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Version *version = data;
   Gotham_Citizen *citizen = ev;
   Eina_Bool updated;

   EINA_SAFETY_ON_NULL_RETURN_VAL(version, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(citizen, EINA_TRUE);

   DBG("version[%p] citizen[%p]=“%s” type %d",
       version, citizen, citizen->jid, citizen->type);

   if (version->gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if ((!version->gotham->alfred) ||
       (citizen->type != GOTHAM_CITIZEN_TYPE_ALFRED) ||
       (version->sent_once))
     return EINA_TRUE;

   version_botman_fetch(version, NULL, &updated);
   version->sent_once = EINA_TRUE;
   return EINA_TRUE;
}

/**
 * @brief Callback when we are connected.
 * If DB info are already loaded, set and send vCard using
 * @ref botman_vcard_send
 * @param data Module_Asp64 structure
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_connect(void *data,
              int type EINA_UNUSED,
              void *ev EINA_UNUSED)
{
   Module_Version *version = data;
   Eina_Bool updated;

   DBG("version[%p]", version);

   if (version->gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   version->poll = ecore_timer_add(60.0, version_botman_poll, version);

   if ((!version->gotham->alfred) ||
       (version->gotham->alfred->status == GOTHAM_CITIZEN_STATUS_OFFLINE))
     return EINA_TRUE;

   version_botman_fetch(version, NULL, &updated);
   version->sent_once = EINA_TRUE;
   return EINA_TRUE;
}

#undef AUTH

/**
 * @}
 */

