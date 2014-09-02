#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "alfred.h"

Eina_Bool
alfred_event_connect(void *data,
                     int type EINA_UNUSED,
                     void *ev)
{
   Gotham *gotham = ev;
   DBG("data[%p] gotham[%p]", data, gotham);

   /**
    * Set Avatar and nickname
    */
   gotham_avatar_data_set(gotham,
                          AVATAR,
                          "image/png");
   gotham_nickname_set(gotham, "Alfred Pennyworth");
   gotham_vcard_send(gotham);

   return EINA_TRUE;
}

Eina_Bool
alfred_event_citizen_command(void *data,
                             int type EINA_UNUSED,
                             void *ev)
{
   Gotham_Citizen_Command *command = ev;

   DBG("data[%p] command[%p]=%s", data, command, command->message);

   /*if (!strcmp(command->name, "lol"))
     {
        char *p = NULL;
        *p = 0;
     }*/

   return EINA_TRUE;
}
