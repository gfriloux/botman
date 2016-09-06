#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gotham_private.h"

/**
 * @addtogroup Libgotham_Gotham_Functions
 * @{
 */

int GOTHAM_EVENT_CONNECT = 0,                /*!< event type for connection  */
    GOTHAM_EVENT_DISCONNECT = 0,             /*!< event type for disconnect  */
    GOTHAM_EVENT_MODULES_SINGLE_READY = 0,   /*!< event type when a single
                                                  module is loaded           */
    GOTHAM_EVENT_MODULES_READY = 0,          /*!< event type when all modules
                                                  are loaded                 */
    GOTHAM_EVENT_CITIZEN_CONNECT = 0,        /*!< event type for citizen cnx */
    GOTHAM_EVENT_CITIZEN_DISCONNECT = 0,     /*!< event type for citizen
                                                  disconnection              */
    GOTHAM_EVENT_CITIZEN_COMMAND = 0,        /*!< event type for commands    */
    GOTHAM_EVENT_CITIZEN_LIST = 0,           /*!< ?                          */
    GOTHAM_EVENT_CITIZEN_INFO = 0,           /*!< ?                          */
    GOTHAM_EVENT_CITIZEN_IDLE = 0,           /*!< ?                          */
    GOTHAM_EVENT_CITIZEN_NEW = 0,            /*!< ?                          */
    GOTHAM_EVENT_CITIZEN_LEAVE = 0;          /*!< ?                          */
static int _gotham_init_count = 0;           /*!< count gotham inits         */


/**
 * Initialize the gotham library
 * Register log domain using eina,
 * creates the ecore_event_types for all needeed ecore_event_handlers
 * When gotham is not used anymore, call gotham_sutdown() to shut down the
 * gotham library.
 * @return 1 or greater on success, 0 on error.
 */
int gotham_init(void)
{
   if (++_gotham_init_count != 1)
     return _gotham_init_count;

   eina_init();
   eet_init();
   ecore_init();
   shotgun_init();
   azy_init();

   gotham_log_dom = eina_log_domain_register("gotham", EINA_COLOR_YELLOW);

   GOTHAM_EVENT_CONNECT = ecore_event_type_new();
   GOTHAM_EVENT_DISCONNECT = ecore_event_type_new();
   GOTHAM_EVENT_MODULES_SINGLE_READY = ecore_event_type_new();
   GOTHAM_EVENT_MODULES_READY = ecore_event_type_new();
   GOTHAM_EVENT_CITIZEN_CONNECT = ecore_event_type_new();
   GOTHAM_EVENT_CITIZEN_DISCONNECT = ecore_event_type_new();
   GOTHAM_EVENT_CITIZEN_COMMAND = ecore_event_type_new();
   GOTHAM_EVENT_CITIZEN_LIST = ecore_event_type_new();
   GOTHAM_EVENT_CITIZEN_INFO = ecore_event_type_new();
   GOTHAM_EVENT_CITIZEN_IDLE = ecore_event_type_new();
   GOTHAM_EVENT_CITIZEN_NEW = ecore_event_type_new();
   GOTHAM_EVENT_CITIZEN_LEAVE = ecore_event_type_new();

   return 1;
}

/**
 * Shut down the Gotham library
 * This function shuts down the Gotham library.
 */
void gotham_shutdown(void)
{
   DBG("void");
   eina_log_domain_unregister(gotham_log_dom);
}

/**
 * Defines if gotham sould automatically reconnect to XMPP server
 * when disconnect happens.
 * @param gotham Gotham object
 * @param bool Eina_Bool value to use
 */
void
gotham_reconnect_set(Gotham *gotham, Eina_Bool bool)
{
   gotham->con.reconnect = bool;
}


/**
 * Creates a new Gotham object.
 * Loads conf file into gotham->conf, and use these informations
 * to connect to xmpp server.
 * Create ecore_event_handlers for connect / disconnect, message, iq, presence
 * Dynamically load modules listed in conf file.
 * @param type Gotham_Citizen_Type
 * @param conf_file Config file path. JSON file
 * @return Newly created structure
 */
Gotham *
gotham_new(Gotham_Citizen_Type type,
           const char *conf_file)
{
   Gotham *gotham;
   Gotham_Module *gotham_module;
   Shotgun_User_Info *user_info;
   const char *jid;

   DBG("type=%d conf_file=%s", type, conf_file);

   EINA_SAFETY_ON_NULL_RETURN_VAL(conf_file, NULL);

   if ((type != GOTHAM_CITIZEN_TYPE_BOTMAN) &&
       (type != GOTHAM_CITIZEN_TYPE_ALFRED))
     {
        ERR("Wrong citizen type!");
        return NULL;
     }

   gotham = calloc(1, sizeof(Gotham));
   gotham->conf = gotham_serialize_file_to_struct(conf_file,  (Gotham_Deserialization_Function)azy_value_to_Gotham_Conf);
   if (!gotham->conf)
     {
        ERR("Failed to load configuration");
        free(gotham);
        return NULL;
     }

   gotham->citizens = eina_hash_string_superfast_new(_gotham_citizen_list_free_cb);

   /* Create our own citizen, aka : me */
   jid = eina_stringshare_printf("%s@%s",
                                 gotham->conf->xmpp->login,
                                 gotham->conf->xmpp->server);
   gotham->me = gotham_citizen_new(gotham, jid);
   eina_stringshare_del(jid);
   if (!gotham->me)
     {
        ERR("Unable to create me as a citizen");
     }
   else
     {
        gotham->me->type = type;
        eina_hash_direct_add(gotham->citizens, gotham->me->jid, gotham->me);
     }

   if ((type == GOTHAM_CITIZEN_TYPE_BOTMAN) &&
       (gotham->conf->xmpp->alfred))
     {
        jid = eina_stringshare_printf("%s@%s",
                                      gotham->conf->xmpp->alfred,
                                      gotham->conf->xmpp->server);
        gotham->alfred = gotham_citizen_new(gotham, jid);
        eina_hash_direct_add(gotham->citizens, gotham->alfred->jid, gotham->alfred);
        eina_stringshare_del(jid);
        if (!gotham->alfred)
          {
             ERR("Unable to create alfred citizen");
          }
        gotham->alfred->type = GOTHAM_CITIZEN_TYPE_ALFRED;
     }

#define EVADD(type, func, obj)                                                 \
   ecore_event_handler_add(SHOTGUN_EVENT_##type,                               \
                           (Ecore_Event_Handler_Cb)_gotham_event_##func,       \
                           obj);
   EVADD(CONNECT, connect, gotham);
   EVADD(DISCONNECT, disconnect, gotham);
   EVADD(MESSAGE, message, gotham);
   EVADD(IQ, iq, gotham);
   EVADD(PRESENCE, presence, gotham);
#undef EVADD


   gotham->shotgun = shotgun_new(gotham->conf->xmpp->server,
                                 gotham->conf->xmpp->login,
                                 gotham->conf->xmpp->server);
   if (!gotham->shotgun)
     {
        ERR("Failed to create shotgun object");
        Gotham_Conf_free(gotham->conf);
        free(gotham);
        return NULL;
     }

   shotgun_resource_set(gotham->shotgun, gotham->conf->xmpp->resource);
   shotgun_password_set(gotham->shotgun, gotham->conf->xmpp->passwd);
   shotgun_ping_delay_set(gotham->shotgun, 20);
   shotgun_ping_timeout_set(gotham->shotgun, 18);
   shotgun_ping_max_attempts_set(gotham->shotgun, 3);

   DBG("Connect to %s as %s",
       gotham->conf->xmpp->server, gotham->conf->xmpp->login);

   if (!shotgun_connect(gotham->shotgun))
     {
        ERR("shotgun_connect failed");
        Gotham_Conf_free(gotham->conf);
        shotgun_free(gotham->shotgun);
        free(gotham);
        return NULL;
     }

   _gotham_modules_load(gotham);

   EINA_INLIST_FOREACH(gotham_modules_list(), gotham_module)
     gotham_modules_register(gotham, gotham_module);

   ecore_event_add(GOTHAM_EVENT_MODULES_READY, gotham,
                   _gotham_fake_free, NULL);

   /**
    * Set jid in Shotgun_User_Info structure
    */
   user_info = calloc(1, sizeof(Shotgun_User_Info));
   user_info->jid = eina_stringshare_add(gotham->me->jid);
   shotgun_vcard_set(gotham->shotgun, user_info);

   DBG("Returning new gotham[%p]", gotham);
   return gotham;
}

/**
 * Free a gotham structure.
 * @param gotham Gotham structure to free
 */
void gotham_free(Gotham *gotham)
{
   DBG("gotham[%p]", gotham);

   EINA_SAFETY_ON_NULL_RETURN(gotham);

   if (gotham->con.connected)
     shotgun_disconnect(gotham->shotgun);

   shotgun_free(gotham->shotgun);
   Gotham_Conf_free(gotham->conf);

   eina_hash_free(gotham->citizens);
   free(gotham);
}

/**
 * Sets an avatar to current account.
 * Reads binary file, encodes it to base 64 and sets it as avatar.
 * @param gotham Gotham structure
 * @param avatar_file Path to file
 * @param mime_type Mime_type of given file
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_avatar_file_set(Gotham *gotham,
                       const char *avatar_file,
                       const char *mime_type)
{
   FILE *fi = NULL;
   char *buffer = NULL,
        *avatar_data;
   struct stat st_img;
   size_t size = 0,
          len = 0,
          readed;
   Eina_Bool ret = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(gotham, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(avatar_file, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(mime_type, EINA_FALSE);

   if (stat(avatar_file, &st_img))
     {
        ERR("Unable to open file “%s”", avatar_file);
        return EINA_FALSE;
     }

   len = (st_img.st_size+1);
   DBG("File size : %zd", (st_img.st_size));
   if (!(buffer = calloc(1, sizeof(char)*(st_img.st_size+1))))
     {
        ERR("calloc failed !");
        return EINA_FALSE;
     }

   fi = fopen(avatar_file, "rb");
   readed = fread(buffer, (st_img.st_size), 1, fi);

   if ((!readed) && (ferror(fi)))
     {
        ERR("Error occured on fread()");
        fclose(fi);
        return EINA_FALSE;
     }
   fclose(fi);

   avatar_data = shotgun_base64_encode((const unsigned char *)buffer, len, &size);
   DBG("encoded data : size=%zd, strlen(enc_data)=%zd",
       size, strlen(avatar_data));
   free(buffer);

   ret = gotham_avatar_data_set(gotham, avatar_data, mime_type);
   free(avatar_data);
   return ret;

}

/**
 * Sets an avatar to current account.
 * @param gotham Gotham structure
 * @param avatar_data Avatar file content, encoded in base64
 * @param mime_type Mime_type of avatar file
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_avatar_data_set(Gotham *gotham,
                       const char *avatar_data,
                       const char *mime_type)
{
   Shotgun_User_Info *vcard;

   EINA_SAFETY_ON_NULL_RETURN_VAL(gotham, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(avatar_data, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(mime_type, EINA_FALSE);

   vcard = shotgun_vcard_get(gotham->shotgun);
   if (!vcard)
     vcard = calloc(1, sizeof(Shotgun_User_Info));
   else
     {
        if (vcard->photo.data) free(vcard->photo.data);
        if (vcard->photo.type) eina_stringshare_replace(&vcard->photo.type, NULL);
     }

   vcard->photo.data = strdup(avatar_data);
   vcard->photo.type = eina_stringshare_add(mime_type);
   shotgun_vcard_set(gotham->shotgun, vcard);

   return EINA_TRUE;

}

/**
 * Sets the current account's nickname
 * @param gotham Gotham structure
 * @param nickname Nickname to set
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_nickname_set(Gotham *gotham,
                    const char *nickname)
{
   Shotgun_User_Info *vcard;

   EINA_SAFETY_ON_NULL_RETURN_VAL(gotham, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(nickname, EINA_FALSE);

   vcard = shotgun_vcard_get(gotham->shotgun);
   if (!vcard)
     {
        vcard = calloc(1, sizeof(Shotgun_User_Info));
        vcard->full_name = eina_stringshare_add(nickname);
     }
   else
     eina_stringshare_replace(&vcard->full_name, nickname);

   if (!gotham->me->nickname)
     gotham->me->nickname = eina_stringshare_add(nickname);
   else
    eina_stringshare_replace(&gotham->me->nickname, nickname);

   shotgun_vcard_set(gotham->shotgun, vcard);
   return EINA_TRUE;
}

/**
 * Sends the vCard to xmpp server
 * @param gotham Gotham structure
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_vcard_send(Gotham *gotham)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(gotham, EINA_FALSE);
   shotgun_iq_vcard_send(gotham->shotgun);
   return EINA_TRUE;
}

/**
 * @}
 */
