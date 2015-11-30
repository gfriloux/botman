#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gotham_private.h"

/**
 * @addtogroup Libgotham_Event_Functions
 * @{
 */

/**
 * XMPP namespace for XHTML protocol
 */
#define XML_NS_XHTML "http://jabber.org/protocol/xhtml-im"

/**
 * XMPP namespace for Last Activity protocol
 */
#define XML_NS_LAST "jabber:iq:last"

/**
 * Callback event on connection. Initialize client and send presence to server
 * @param data Callback data (Gotham structure)
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
_gotham_event_connect(void *data,
                      int type EINA_UNUSED,
                      void *ev EINA_UNUSED)
{
   Gotham *gotham = data;

   DBG("gotham[%p]", gotham);

   EINA_SAFETY_ON_NULL_RETURN_VAL(gotham, EINA_TRUE);

   gotham->con.connected = EINA_TRUE;

   ecore_event_add(GOTHAM_EVENT_CONNECT, gotham, _gotham_fake_free, NULL);

   shotgun_iq_server_query(gotham->shotgun);
   shotgun_iq_roster_get(gotham->shotgun);
   shotgun_presence_send(gotham->shotgun);


   return EINA_TRUE;
}

/**
 * Occurs if reconnect has been set, and after a disconnection.
 * Connects again to xmpp server
 * @param data Gotham structure
 * @return EINA_TRUE
 */
Eina_Bool
_gotham_event_reconnect(void *data)
{
   Gotham *gotham = data;
   DBG("gotham[%p]", gotham);

   ecore_timer_del(gotham->con.recon);
   shotgun_connect(gotham->shotgun);

   return EINA_TRUE;
}

/**
 * Callback event for disconnection.
 * If reconnect is set, start timer (reconnection will occur 10s later)
 * @param data Callback data (Gotham structure)
 * @param type
 * @param ev
 * @return EINA_TRUE
 */
Eina_Bool
_gotham_event_disconnect(void *data,
                         int type EINA_UNUSED,
                         void *ev EINA_UNUSED)
{
   Gotham *gotham = data;

   DBG("gotham[%p]", gotham);

   EINA_SAFETY_ON_NULL_RETURN_VAL(gotham, EINA_TRUE);

   gotham->con.connected = EINA_FALSE;

   ecore_event_add(GOTHAM_EVENT_DISCONNECT, gotham, _gotham_fake_free, NULL);

   if (gotham->con.reconnect)
     {
        gotham->con.recon = ecore_timer_add(10,
                                            _gotham_event_reconnect,
                                            gotham);
     }

   return EINA_TRUE;
}

/**
 * Free a Gotham_Citizen_Command when not used anymore
 * @param data
 * @param ev Event data representing a Gotham_Citizen_Command struct
 */
void
_gotham_event_message_free(void *data EINA_UNUSED,
                           void *ev)
{
   Gotham_Citizen_Command *command = ev;
   Gotham *gotham = command->citizen->gotham;
   int citizen_sum;
   DBG("command[%p]", command);

   /* A module has taken care of this command */
   if (command->handled)
     goto func_end;

   /* No module answered this command, we have to inform sender */
   NFO("Command %s was not handled by any module", command->name);

   /**
    * Neither Alfred and Botman, nor 2 Botmans, should answer
    * such things each other, or they would be stucked in that
    * pointless disussion for ever
    */
   citizen_sum = gotham->me->type + command->citizen->type;

   if ((citizen_sum == (GOTHAM_CITIZEN_TYPE_BOTMAN +
                        GOTHAM_CITIZEN_TYPE_ALFRED)) ||
       (citizen_sum == (2 * GOTHAM_CITIZEN_TYPE_BOTMAN)))
     goto func_end;

   shotgun_message_send(gotham->shotgun,
                        command->citizen->jid,
                        "Unknown command",
                        SHOTGUN_MESSAGE_STATUS_ACTIVE,
                        EINA_FALSE);

func_end:
   gotham_command_free(command);
}

/**
 * Trigger a GOTHAM_EVENT_CITIZEN_COMMAND event.
 * Adds the matching Gotham_Citizen_Command data to this event.
 * @param command Gotham_Citizen_Command
 */
void
gotham_event_command_new(Gotham_Citizen_Command *command)
{
   ecore_event_add(GOTHAM_EVENT_CITIZEN_COMMAND, command,
                   _gotham_event_message_free, NULL);
}

/**
 * Callback event when receiving a message.
 * Parse incoming JID and seach it in citizen list.
 * Messages can only be sent to friends. If citizen is found,
 * create a new Gotham_Citizen_Command and trigger the matching
 * event.
 * @param data Gotham structure
 * @param type
 * @param ev Shotgun_Event_Message (will be turned in a Gotham message)
 * @return EINA_TRUE
 */
Eina_Bool
_gotham_event_message(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Shotgun_Event_Message *msg = ev;
   Gotham *gotham = data;
   Gotham_Citizen *citizen;
   Gotham_Citizen_Command *command;
   const char *p,
              *jid,
              *resource;


   DBG("gotham[%p] msg[%p]", gotham, msg);

   EINA_SAFETY_ON_NULL_RETURN_VAL(msg, EINA_TRUE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_TRUE);

   if (!msg->msg)
     return EINA_TRUE;

   DBG("Parsing jid %s", msg->jid);
   p = strchr(msg->jid, '/');
   if (!p)
     {
        ERR("WTF ? JID %s Without a '/' ?", msg->jid);
        return EINA_TRUE;
     }

   jid = eina_stringshare_add_length(msg->jid, p - msg->jid);
   resource = eina_stringshare_add(p + 1);

   citizen = eina_hash_find(gotham->citizens, msg->jid);
   citizen = eina_hash_find(gotham->citizens, jid);
   eina_stringshare_del(jid);
   if (!citizen)
     {
        ERR("Message from unknown user %s", msg->jid);
        return EINA_TRUE;
     }

   //if (citizen->type == GOTHAM_CITIZEN_TYPE_UNIDENTIFIED)
   //  {
        if (!strncmp(resource, "botman", 6))
          {
             DBG("SETTING %s AS BEING A BOTMAN", citizen->jid);
             citizen->type = GOTHAM_CITIZEN_TYPE_BOTMAN;
          }
        else if (gotham->alfred)
          {
             if (strncmp(gotham->alfred->jid, citizen->jid, strlen(gotham->alfred->jid)))
               citizen->type = GOTHAM_CITIZEN_TYPE_CIVILIAN;
          }
        else citizen->type = GOTHAM_CITIZEN_TYPE_CIVILIAN;
        eina_stringshare_del(resource);
   //  }

   command = gotham_command_new(citizen, msg->msg, msg->jid);
   gotham_event_command_new(command);

   return EINA_TRUE;
}

/**
 * Callback event for an IQ.
 * Depending on the SHOTGUN_IQ_EVENT_TYPE type, we handle it (or not)
 * Can be :
 * @li ROSTER : when a friend comes online. Add him to citizen list
 * if needed; send a vcard request and eventually an activity query
 * @li INFO : when a friend changes information (ie nickname, avatar...)
 * @li IDLE : when a friend goes to idle state
 * @li DISCO_QUERY : when a friend sends his available features
 * We do not handle other cases since we don't need them.
 * @param data Gotham structure
 * @param type
 * @param ev Shotgun_Event_Iq
 * @return EINA_TRUE
 */
Eina_Bool
_gotham_event_iq(void *data,
                 int type EINA_UNUSED,
                 void *ev)
{
   Gotham *gotham = data;
   Shotgun_Event_Iq *iq = ev;
   Gotham_Citizen *citizen;

   DBG("gotham[%p] conf[%p]", gotham, gotham->conf);

   switch(iq->type)
     {
      case SHOTGUN_IQ_EVENT_TYPE_ROSTER:
        {
           Shotgun_User *user;
           Eina_List *l;
           DBG("gotham[%p] iq[%p] SHOTGUN_IQ_EVENT_TYPE_ROSTER",
               gotham, iq);

           EINA_LIST_FOREACH(iq->ev, l, user)
             {
                DBG("ROSTER from %s, subscription type %d (pending ? %d)",
                    user->jid, user->subscription, user->subscription_pending);

                citizen = eina_hash_find(gotham->citizens, user->jid);

                /* Remove user from list if subscription is not BOTH */
                if (user->subscription == SHOTGUN_USER_SUBSCRIPTION_NONE)
                  {
                     if (!citizen)
                       continue;
/*
                     DBG("Removing citizen %s from citizen list", citizen->jid);
                     eina_hash_del(gotham->citizens, NULL, citizen);
*/
                     continue;
                  }

                /* only add to citizen list if subscription BOTH */
                if (!citizen)
                  {
                     citizen = gotham_citizen_new(gotham, user->jid);
                     if (!citizen)
                       {
                          ERR("Failed to create new citizen %s !", user->jid);
                          break;
                       }
                     DBG("Adding citizen %s to citizen list", citizen->jid);
                     eina_hash_direct_add(gotham->citizens,
                                          user->jid,
                                          citizen);
                  }
                else
                  citizen->subscribed = ((!user->subscription_pending) &&
                     (user->subscription >= SHOTGUN_USER_SUBSCRIPTION_FROM));

                /* Known user, request vcard and last activity */
                shotgun_iq_vcard_get(iq->account, user->jid);
                if (citizen->features.last == EINA_TRUE)
                  shotgun_iq_activity_query(iq->account, user->jid);
             }

           ecore_event_add(GOTHAM_EVENT_CITIZEN_LIST, gotham,
                           _gotham_fake_free, NULL);
           break;
        }
      case SHOTGUN_IQ_EVENT_TYPE_INFO:
        {
           Shotgun_User_Info *info = iq->ev;

           DBG("gotham[%p] iq[%p] SHOTGUN_IQ_EVENT_TYPE_INFO",
               gotham, iq);

           citizen = eina_hash_find(gotham->citizens, info->jid);
           if (!citizen)
             {
                ERR("Receiving user changes, about an unknown user %s!",
                    info->jid);
                break;
             }
           eina_stringshare_replace(&citizen->nickname, info->full_name);
           ecore_event_add(GOTHAM_EVENT_CITIZEN_INFO, citizen,
                           _gotham_fake_free, NULL);
           break;
        }
      case SHOTGUN_IQ_EVENT_TYPE_IDLE:
        {
           Shotgun_Iq_Last *last = iq->ev;
           DBG("gotham[%p] iq[%p] SHOTGUN_IQ_EVENT_TYPE_IDLE",
               gotham, iq);

           DBG("Last activity for user %s : %d seconds\n",
                  last->jid, last->last);
           /*  add GOTHAM_EVENT_CITIZEN_IDLE event */
           break;
        }
      case SHOTGUN_IQ_EVENT_TYPE_DISCO_QUERY:
        {
           Shotgun_Iq_Disco *disco = iq->ev;
           const char *ptr,
                      *jid;
           Eina_List *l;

           ptr = strchr(disco->jid, '/');
           if (!ptr)
             {
                ERR("Malformed JID %s", disco->jid);
                return EINA_TRUE;
             }

           jid = eina_stringshare_add_length(disco->jid, ptr - disco->jid);
           DBG("Getting user info (features discovery) from “%s”", jid);
           citizen = eina_hash_find(gotham->citizens, jid);
           eina_stringshare_del(jid);
           if (!citizen)
             break;

           DBG("gotham[%p] iq[%p] SHOTGUN_IQ_EVENT_TYPE_DISCO_QUERY",
               gotham, iq);

           EINA_LIST_FOREACH(disco->features, l, ptr)
             {
                if (!strncmp(ptr, XML_NS_XHTML, strlen(XML_NS_XHTML)))
                  {
                     citizen->features.xhtml = EINA_TRUE;
                     continue;
                  }
                if (!strncmp(ptr, XML_NS_LAST, strlen(XML_NS_LAST)))
                  {
                     citizen->features.last = EINA_TRUE;
                     continue;
                  }
             }
           if (citizen->features.last)
             shotgun_iq_activity_query(citizen->gotham->shotgun, disco->jid);
           break;
        }
      case SHOTGUN_IQ_EVENT_TYPE_SERVER_QUERY:
      case SHOTGUN_IQ_EVENT_TYPE_SETTINGS:
      case SHOTGUN_IQ_EVENT_TYPE_MAILNOTIFY:
      case SHOTGUN_IQ_EVENT_TYPE_OTR_QUERY:
      case SHOTGUN_IQ_EVENT_TYPE_ARCHIVE_COLLECTION:
      case SHOTGUN_IQ_EVENT_TYPE_FILE_TRANSFER:
      case SHOTGUN_IQ_EVENT_TYPE_BYTESTREAM:
      case SHOTGUN_IQ_EVENT_TYPE_IBB:
      case SHOTGUN_IQ_EVENT_TYPE_PING:
      case SHOTGUN_IQ_EVENT_TYPE_BOB:
      case SHOTGUN_IQ_EVENT_TYPE_UNKNOWN:
      default:
        {
           printf("WTF!\n");
           break;
        }
     }
   return EINA_TRUE;
}

/**
 * Free.... something about online presence
 * @param d thing to free
 * @param d2 UNUSED
 */
void
_gotham_event_presence_free(void *d, void *d2 EINA_UNUSED)
{
   free(d);
}

/**
 * Callback for presence notification. Received in different cases :
 * @li SHOTGUN_PRESENCE_TYPE_SUBSCRIBE : a user wants to be our friend.
 * He'll be allowed to if he matches the autorized citizen pattern(s)
 * We also send a friend request to this fellow. He will be added to citizen list
 * in a future IQ event
 * @li SHOTGUN_PRESENCE_TYPE_UNSUBSCRIBE : when a citizen doesn't want
 * to be our friend anymore. Remove him from citizen, and remove him from
 * our friends list
 * @li SHOTGUN_PRESENCE_TYPE_UNAVAILABLE : don't care about this.
 * @li all other cases : update the citizen's status (ONLIE / OFFLINE)
 * @param data Gotham structure
 * @param type
 * @param ev Shotgun_Event_Presence
 * @return EINA_TRUE
 */
Eina_Bool
_gotham_event_presence(void *data,
                       int type EINA_UNUSED,
                       void *ev)
{
   Shotgun_Event_Presence *presence = ev;
   Gotham *gotham = data;
   Gotham_Citizen *citizen;
   const char *ptr,
              *jid,
              *resource;

   DBG("gotham[%p] presence[%p] from %s, type %d",
       gotham, presence, presence->jid, presence->type);

   /* We dont care about ourself */
   if (!strncmp(presence->jid, gotham->conf->xmpp.login,
                strlen(gotham->conf->xmpp.login)))
     return EINA_TRUE;

   if (presence->type == SHOTGUN_PRESENCE_TYPE_SUBSCRIBE)
     {
        if (!_gotham_citizen_authorized(gotham, presence->jid))
          {
             NFO("User “%s” is not allowed to subscribe", presence->jid);
             shotgun_message_send(gotham->shotgun,
                                  presence->jid,
                                  "We are not friends.",
                                  SHOTGUN_MESSAGE_STATUS_ACTIVE,
                                  EINA_FALSE);
             shotgun_presence_subscription_answer_set(gotham->shotgun,
                                                      presence->jid,
                                                      EINA_FALSE);
             return ECORE_CALLBACK_RENEW;
          }
        NFO("Adding “%s” to our contact list", presence->jid);
        shotgun_presence_subscription_answer_set(gotham->shotgun,
                                                 presence->jid,
                                                 EINA_TRUE);
        shotgun_presence_subscription_set(gotham->shotgun,
                                          presence->jid,
                                          EINA_TRUE);
        return ECORE_CALLBACK_RENEW;
     }
   else if (presence->type == SHOTGUN_PRESENCE_TYPE_UNSUBSCRIBE)
     {
        NFO("User %s deleted me, this is chocking!", presence->jid);
        shotgun_presence_subscription_answer_set(gotham->shotgun,
                                                 presence->jid,
                                                 EINA_FALSE);
        citizen = eina_hash_find(gotham->citizens, presence->jid);
        if (citizen)
          eina_hash_del(gotham->citizens, NULL, citizen);
        return ECORE_CALLBACK_RENEW;
     }

   ptr = strchr(presence->jid, '/');
   if (!ptr)
     {
        ERR("Malformed JID %s", presence->jid);
        return EINA_TRUE;
     }

   jid = eina_stringshare_add_length(presence->jid, ptr - presence->jid);
   resource = eina_stringshare_add(ptr + 1);

   citizen = eina_hash_find(gotham->citizens, jid);

   if (!citizen)
     {
        ERR("Holy Shit! Receiving presence about unknown user %s!",
            jid);
        eina_stringshare_del(jid);
        eina_stringshare_del(resource);
        return EINA_TRUE;
     }

   DBG("Citizen[%p] : %s", citizen, citizen->jid);

   if ((citizen->type == GOTHAM_CITIZEN_TYPE_UNIDENTIFIED) &&
       (presence->status != SHOTGUN_USER_STATUS_NONE) &&
       (!strncmp(resource, "botman", 6)))
     {
        DBG("SETTING %s AS BEING A BOTMAN", citizen->jid);
        citizen->type = GOTHAM_CITIZEN_TYPE_BOTMAN;
     }

   if (presence->type != 0)
     {
        citizen->status = GOTHAM_CITIZEN_STATUS_OFFLINE;
        ecore_event_add(GOTHAM_EVENT_CITIZEN_DISCONNECT, citizen,
                        _gotham_fake_free, NULL);
     }
   else
     {
        citizen->status = GOTHAM_CITIZEN_STATUS_ONLINE;
        shotgun_iq_disco_info_get(citizen->gotham->shotgun, presence->jid);
        ecore_event_add(GOTHAM_EVENT_CITIZEN_CONNECT, citizen,
                        _gotham_fake_free, NULL);
     }

   eina_stringshare_del(jid);
   eina_stringshare_del(resource);

   return EINA_TRUE;
}

/**
 * @}
 */
