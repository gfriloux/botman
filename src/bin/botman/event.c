#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "botman.h"
#include <unistd.h>

Eina_Bool alfred_pending = EINA_FALSE;

void
_botman_fake_free(void *d EINA_UNUSED, void *d2 EINA_UNUSED)
{}

Eina_Bool
botman_event_connect(void *data EINA_UNUSED,
                     int type EINA_UNUSED,
                     void *ev)
{
   DBG("ev[%p]", ev);
   connected = EINA_TRUE;
   return EINA_TRUE;
}

Eina_Bool
botman_event_disconnect(void *data EINA_UNUSED,
                        int type EINA_UNUSED,
                        void *ev)
{
   DBG("ev[%p]", ev);

   if (!connected)
     {
        sleep(10);
#ifndef _WIN32
        ecore_app_restart();
#else
        ecore_main_loop_quit();
#endif
     }
   else
     connected = EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
botman_event_citizen_command(void *data,
                             int type EINA_UNUSED,
                             void *ev)
{
   Gotham_Citizen_Command *command = ev;
   DBG("data[%p] command[%p]", data, command);
   /*if (!strcmp(command->name, "lol"))
     {
        char *p = NULL;
        *p = 0;
     }*/
   return EINA_TRUE;
}


Eina_Bool
botman_event_citizen_list(void *data,
                          int type EINA_UNUSED,
                          void *ev)
{
   Gotham *gotham = ev;
   Gotham_Citizen *citizen;

   DBG("data[%p] gotham[%p]", data, gotham);

   /* Get alfred */
   citizen = eina_hash_find(gotham->citizens, gotham->alfred->jid);
   if (!citizen)
     {
        ERR("Failed to find alfred in citizen list.");
        return EINA_TRUE;
     }

   if (alfred_pending)
     {
        DBG("Request already sent to Alfred");
        return EINA_TRUE;
     }

   if (citizen->subscribed)
     return EINA_TRUE;

   DBG("Where is Alfred ? Alfred !");
   shotgun_presence_subscription_set(gotham->shotgun,
                                     gotham->alfred->jid,
                                     EINA_TRUE);
   alfred_pending = EINA_TRUE;
   DBG("Subscription sent");

   return EINA_TRUE;
}

Eina_Bool
botman_event_citizen_connect(void *data,
                             int type EINA_UNUSED,
                             void *ev)
{
   Gotham_Citizen *citizen = ev;
   Gotham *gotham = citizen->gotham;

   DBG("data[%p] gotham[%p]", data, gotham);
   NFO("Citizen %s just connected", citizen->jid);

   if (citizen->type != GOTHAM_CITIZEN_TYPE_ALFRED)
     return EINA_TRUE;

   if (!citizen->subscribed)
     {
        NFO("Alfred found but not subscribed yet");
        return EINA_TRUE;
     }

   DBG("Alfred was just added, calling MODULES_READY event");
   ecore_event_add(GOTHAM_EVENT_MODULES_READY, gotham,
                   _botman_fake_free, NULL);

   return EINA_TRUE;
}
