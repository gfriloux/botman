#ifndef GOTHAM
#define GOTHAM
#include <Eina.h>
#include <Ecore.h>
#include <Azy.h>
#include <Shotgun.h>
#include <cJSON.h>

#ifdef GOTHAM_API
# undef GOTHAM_API
#endif

#ifdef _WIN32
# ifdef EFL_GOTHAM_BUILD
#  ifdef DLL_EXPORT
#   define GOTHAM_API __declspec(dllexport)
#  else
#   define GOTHAM_API
#  endif /* ! DLL_EXPORT */
# else
#  define GOTHAM_API __declspec(dllimport)
# endif /* ! EFL_GOTHAM_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define GOTHAM_API __attribute__ ((visibility("default")))
#  else
#   define GOTHAM_API
#  endif
# else
#  define GOTHAM_API
# endif
#endif /* ! _WIN32 */

GOTHAM_API extern int GOTHAM_EVENT_CONNECT;
GOTHAM_API extern int GOTHAM_EVENT_DISCONNECT;
GOTHAM_API extern int GOTHAM_EVENT_MODULES_SINGLE_READY;
GOTHAM_API extern int GOTHAM_EVENT_MODULES_READY;
GOTHAM_API extern int GOTHAM_EVENT_CITIZEN_CONNECT;
GOTHAM_API extern int GOTHAM_EVENT_CITIZEN_DISCONNECT;
GOTHAM_API extern int GOTHAM_EVENT_CITIZEN_COMMAND;
GOTHAM_API extern int GOTHAM_EVENT_CITIZEN_LIST;
GOTHAM_API extern int GOTHAM_EVENT_CITIZEN_INFO;
GOTHAM_API extern int GOTHAM_EVENT_CITIZEN_IDLE;
GOTHAM_API extern int GOTHAM_EVENT_CITIZEN_NEW;
GOTHAM_API extern int GOTHAM_EVENT_CITIZEN_LEAVE;

typedef struct _Gotham Gotham;
typedef struct _Gotham_Citizen Gotham_Citizen;
typedef struct _Gotham_Module Gotham_Module;

typedef Eina_Bool (*Gotham_Deserialization_Function)(const Eina_Value *, void **);
typedef Eina_Value * (*Gotham_Serialization_Function)(const void *);

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
              *message,
              *jid;

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

GOTHAM_API Gotham_Citizen *gotham_citizen_new(Gotham *gotham, const char *jid);
GOTHAM_API Eina_Bool gotham_citizen_exist(Gotham *gotham, const char *jid);
GOTHAM_API void gotham_citizen_free(Gotham_Citizen *citizen);
GOTHAM_API Eina_Bool gotham_citizen_send(Gotham_Citizen *citizen, const char *msg);
GOTHAM_API Eina_Bool gotham_citizen_var_set(Gotham_Citizen *citizen, const char *name, const char *value, ...);
GOTHAM_API Eina_Bool gotham_citizen_var_del(Gotham_Citizen *citizen, const char *name);
GOTHAM_API const char *gotham_citizen_var_get(Gotham_Citizen *citizen, const char *name);
GOTHAM_API Eina_Iterator * gotham_citizen_var_iterate(Gotham_Citizen *citizen);
GOTHAM_API Eina_List *gotham_citizen_match(Gotham *gotham, const char *pattern, Gotham_Citizen_Type flags_type, Eina_List *vars);
GOTHAM_API const char * gotham_citizen_match_print(Eina_List *vars, Gotham_Citizen *citizen, Eina_Bool print_presence, Eina_Bool print_lastseen);

GOTHAM_API Gotham_Citizen_Command *gotham_command_new(Gotham_Citizen *citizen, const char *msg, const char *jid);
GOTHAM_API void gotham_command_free(Gotham_Citizen_Command *command);
GOTHAM_API Eina_Bool gotham_command_send(Gotham_Citizen_Command *command, const char *msg);

GOTHAM_API Eina_Bool gotham_emoticons_custom_add(Gotham *gotham, const char *file);

GOTHAM_API void gotham_event_command_new(Gotham_Citizen_Command *command);
GOTHAM_API void gotham_command_json_answer(const char *cmd, const char *params, Eina_Bool status, Eina_Strbuf *content, Gotham *gotham, Gotham_Citizen *citizen, Eina_Bool send_to_alfred);

GOTHAM_API Eina_Inlist *gotham_modules_list(void);
GOTHAM_API Gotham_Module *gotham_modules_single_module_load(const char *name, Gotham *gotham);
GOTHAM_API void gotham_modules_unload(const char *name);
GOTHAM_API Eina_Bool gotham_modules_command_add(const char *module_name, const char *command, const char *desc);
GOTHAM_API void gotham_modules_command_del(const char *module_name, const char *command);
GOTHAM_API Eina_Bool gotham_modules_command_var_set(Gotham_Module_Command *command, const char *name, const char *value, ...);
GOTHAM_API const char *gotham_modules_command_var_get(Gotham_Module_Command *command, const char *name);
GOTHAM_API Gotham_Module_Command *gotham_modules_command_get(const char *cmd);
GOTHAM_API void * gotham_modules_function_get(const char *module, const char *function);
GOTHAM_API void * gotham_modules_data_get(const char *module);
GOTHAM_API void gotham_modules_register(Gotham *gotham, Gotham_Module *module);

GOTHAM_API cJSON *gotham_modules_conf_load(const char *file);
GOTHAM_API Eina_Bool gotham_modules_conf_save(const char *file, cJSON *json);

GOTHAM_API const char **gotham_utils_msgtocommand(const char *msg);
GOTHAM_API const char **gotham_utils_json_array_stringify(cJSON *json_var);
GOTHAM_API char * gotham_utils_file_data_read(const char *file, size_t *size);
GOTHAM_API Eina_Bool gotham_utils_file_data_write(const char *file, const char *data, size_t size);

GOTHAM_API int gotham_init(void);
GOTHAM_API void gotham_free(Gotham *gotham);
GOTHAM_API Gotham *gotham_new(Gotham_Citizen_Type type, const char *conf_file);
GOTHAM_API void gotham_reconnect_set(Gotham *gotham, Eina_Bool bool);
GOTHAM_API void gotham_shutdown(void);

GOTHAM_API Eina_Bool gotham_avatar_file_set(Gotham *gotham, const char *avatar_file, const char *mime_type);
GOTHAM_API Eina_Bool gotham_avatar_data_set(Gotham *gotham, const char *avatar_data, const char *mime_type);
GOTHAM_API Eina_Bool gotham_nickname_set(Gotham *gotham, const char *nickname);
GOTHAM_API Eina_Bool gotham_vcard_send(Gotham *gotham);

GOTHAM_API void * gotham_serialize_string_to_struct(const char *s, size_t len, Gotham_Deserialization_Function func);
GOTHAM_API char * gotham_serialize_struct_to_string(void *data, Gotham_Serialization_Function func);
GOTHAM_API void * gotham_serialize_file_to_struct(const char *file, Gotham_Deserialization_Function func);
GOTHAM_API Eina_Bool gotham_serialize_struct_to_file(void *data, const char *file, Gotham_Serialization_Function func);
