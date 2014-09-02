#include <Eina.h>
#include <Ecore.h>
#include <Shotgun.h>
#include <cJSON.h>

#ifndef GOTHAM
#define GOTHAM
EAPI extern int GOTHAM_EVENT_CONNECT;
EAPI extern int GOTHAM_EVENT_DISCONNECT;
EAPI extern int GOTHAM_EVENT_MODULES_SINGLE_READY;
EAPI extern int GOTHAM_EVENT_MODULES_READY;
EAPI extern int GOTHAM_EVENT_CITIZEN_CONNECT;
EAPI extern int GOTHAM_EVENT_CITIZEN_DISCONNECT;
EAPI extern int GOTHAM_EVENT_CITIZEN_COMMAND;
EAPI extern int GOTHAM_EVENT_CITIZEN_LIST;
EAPI extern int GOTHAM_EVENT_CITIZEN_INFO;
EAPI extern int GOTHAM_EVENT_CITIZEN_IDLE;
EAPI extern int GOTHAM_EVENT_CITIZEN_NEW;
EAPI extern int GOTHAM_EVENT_CITIZEN_LEAVE;

typedef struct _Gotham Gotham;
typedef struct _Gotham_Citizen Gotham_Citizen;
typedef struct _Gotham_Module Gotham_Module;

typedef enum
{
   GOTHAM_CITIZEN_TYPE_UNIDENTIFIED = 1,
   GOTHAM_CITIZEN_TYPE_BOTMAN = 2,
   GOTHAM_CITIZEN_TYPE_ALFRED = 4,
   GOTHAM_CITIZEN_TYPE_CIVILIAN = 8 
} Gotham_Citizen_Type;

typedef enum
{
   GOTHAM_CITIZEN_STATUS_ONLINE,
   GOTHAM_CITIZEN_STATUS_AWAY,
   GOTHAM_CITIZEN_STATUS_BUSY,
   GOTHAM_CITIZEN_STATUS_OFFLINE
} Gotham_Citizen_Status;

typedef struct _Gotham_Conf
{
   struct
   {
      const char *server,
                 *login,
                 *passwd,
                 *resource,
                 *alfred;
   } xmpp;

   Eina_List *modules,
             *friends;
} Gotham_Conf;

struct _Gotham
{
   Gotham_Citizen *me;

   struct
   {
      Eina_Bool connected,
                reconnect;
      Ecore_Timer *recon;
   } con;

   Gotham_Conf *conf;
   Shotgun_Auth *shotgun;
   Eina_Hash *citizens;

   Gotham_Citizen *alfred;
};

struct _Gotham_Citizen
{
   const char *jid,
              *nickname;
   Gotham_Citizen_Type type;
   Gotham_Citizen_Status status;
   Eina_Bool subscribed;

   struct
   {
      Eina_Bool xhtml,
                last;
   } features;

   Gotham *gotham;
   Eina_Hash *vars;

};

typedef struct _Gotham_Citizen_Command
{
   const char *name,
              *parameters,
              *status,
              **command,
              *message;

   Eina_Bool handled : 1;
   Gotham_Citizen *citizen;
} Gotham_Citizen_Command;

typedef const char *(*Gotham_Module_Init)(void);
typedef void *(*Gotham_Module_Register)(Gotham *);
typedef void (*Gotham_Module_Unregister)(void *);
typedef Eina_List *(*Gotham_Module_Commands)(void);
typedef void (*Gotham_Module_Shutdown)(void);
typedef Eina_Bool (*Gotham_Module_Event)(void *, int, void *);

struct _Gotham_Module
{
   EINA_INLIST;
   Eina_Module *module;
   const char *name;
   void *module_data;
   Eina_Inlist *commands;

   Gotham_Module_Init module_init;
   Gotham_Module_Register module_register;
   Gotham_Module_Unregister module_unregister;
   Gotham_Module_Shutdown module_shutdown;

   Eina_List *evs;
};

typedef struct _Gotham_Module_Command
{
   EINA_INLIST;

   const char *command,
              *desc;

   Eina_Hash *vars;
} Gotham_Module_Command;
#endif

Gotham_Citizen *gotham_citizen_new(Gotham *gotham, const char *jid);
Eina_Bool gotham_citizen_exist(Gotham *gotham, const char *jid);
void gotham_citizen_free(Gotham_Citizen *citizen);
Eina_Bool gotham_citizen_send(Gotham_Citizen *citizen, const char *msg);
Eina_Bool gotham_citizen_var_set(Gotham_Citizen *citizen, const char *name, const char *value, ...);
Eina_Bool gotham_citizen_var_del(Gotham_Citizen *citizen, const char *name);
const char *gotham_citizen_var_get(Gotham_Citizen *citizen, const char *name);
Eina_List *gotham_citizen_match(Gotham *gotham, const char *pattern, Gotham_Citizen_Type flags_type, Eina_Array *vars);

Gotham_Citizen_Command *gotham_command_new(Gotham_Citizen *citizen, const char *msg);
void gotham_command_free(Gotham_Citizen_Command *command);

Eina_Bool gotham_emoticons_custom_add(Gotham *gotham, const char *file);

void gotham_event_command_new(Gotham_Citizen_Command *command);

Eina_Inlist *gotham_modules_list(void);
Gotham_Module *gotham_modules_single_module_load(const char *name, Gotham *gotham);
void gotham_modules_unload(const char *name);
Eina_Bool gotham_modules_command_add(const char *module_name, const char *command, const char *desc);
void gotham_modules_command_del(const char *module_name, const char *command);
Eina_Bool gotham_modules_command_var_set(Gotham_Module_Command *command, const char *name, const char *value, ...);
const char *gotham_modules_command_var_get(Gotham_Module_Command *command, const char *name);
Gotham_Module_Command *gotham_modules_command_get(const char *cmd);
void * gotham_modules_function_get(const char *module, const char *function);
void gotham_modules_register(Gotham *gotham, Gotham_Module *module);

cJSON *gotham_modules_conf_load(const char *file);
Eina_Bool gotham_modules_conf_save(const char *file, cJSON *json);

const char **gotham_utils_msgtocommand(const char *msg);
const char **gotham_utils_json_array_stringify(cJSON *json_var);

int gotham_init(void);
void gotham_free(Gotham *gotham);
Gotham *gotham_new(Gotham_Citizen_Type type, const char *conf_file);
void gotham_reconnect_set(Gotham *gotham, Eina_Bool bool);
void gotham_shutdown(void);

Eina_Bool gotham_avatar_file_set(Gotham *gotham, const char *avatar_file, const char *mime_type);
Eina_Bool gotham_avatar_data_set(Gotham *gotham, const char *avatar_data, const char *mime_type);
Eina_Bool gotham_nickname_set(Gotham *gotham, const char *nickname);
Eina_Bool gotham_vcard_send(Gotham *gotham);
