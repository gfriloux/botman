#ifndef HTTPD_H
# define HTTPD_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <Eina.h>
# include <Ecore.h>
# include <Gotham.h>
# include <Azy.h>

# include "Httpd_Common_Azy.h"
# include "Httpd_Common.h"
# include "Httpd_Serve.azy_server.h"

# define MODULE_HTTPD_CONF SYSCONF_DIR"/gotham/modules.conf.d/services.conf"

int _module_httpd_log_dom;

typedef enum _Httpd_Queue_State
{
   QUEUE_COMPLETE,
   QUEUE_INCOMPLETE,
   QUEUE_NO_RESULTS
} Httpd_Queue_State;

typedef void (*Httpd_Uri_Func)(void *module_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);
typedef void (*Httpd_Queue_Cb)(void *data, Httpd_Queue_State state, Eina_List *answers);
typedef void (*Httpd_Queue_Timeout_Cb)(void *data, Eina_List *answers);

typedef struct _Module_Httpd
{
   Gotham *gotham;
   Azy_Server *azy;

   Eina_List *modules;

   struct
   {
      time_t start;

      Eina_List *last_queries;
   } stats;

} Module_Httpd;

typedef struct _Module_Httpd_Module
{
   const char *uri,
              *description;

   struct
   {
      Httpd_Uri_Func func;
      void *data;
   } cb;
} Module_Httpd_Module;

typedef struct _Module_Httpd_Queue
{
   const char *command;

   struct
   {
      Httpd_Queue_Cb func;
      void *data;
   } cb;

   Ecore_Timer *timeout;
   Eina_List *bots;
} Module_Httpd_Queue;

typedef struct _Module_Httpd_Queue_Message
{
   const char *jid,
              *message;
} Module_Httpd_Queue_Message;

Module_Httpd *_httpd;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_httpd_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_httpd_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_httpd_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_httpd_log_dom, __VA_ARGS__)

void httpd_module_add(const char *uri, Httpd_Uri_Func func, void *data, const char *description);
void httpd_module_free(Module_Httpd_Module *mhm);

Module_Httpd_Module * httpd_uri_find(const char *path);

void httpd_queue_init(void);
void httpd_queue_add(Module_Httpd_Queue *mhq);
void httpd_queue_perform(Gotham_Citizen_Command *command);
void httpd_queue_remove(Module_Httpd_Queue *mhq);
void httpd_queue_free(Module_Httpd_Queue *mhq);

void httpd_spam_init(void);
Eina_Bool httpd_spam_new(const char *pattern, const char *command, const char *message, Httpd_Queue_Cb func, void *data);
void httpd_spam(void *spam_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);

void httpd_uri_options(Azy_Server_Module *module, Azy_Net *net);
void httpd_uri_data(Azy_Server_Module *module, char *data, size_t len, int http_code);

void httpd_uri_seen_init(void);
void httpd_uri_seen(void *seen_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);

void httpd_uri_spam_init(void);
void httpd_uri_spam(void *spam_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);

void httpd_uri_stats_command_add(Gotham_Citizen_Command *command);
void httpd_uri_stats_command_list_init(void);
void httpd_uri_stats_command_list(void *stats_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);

void httpd_uri_uptime_init(void);
void httpd_uri_uptime(void *uptime_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);

void httpd_uri_service_init(void);
void httpd_uri_service(void *service_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);

void httpd_uri_stats_init(void);
void httpd_uri_stats(void *memory_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);
void httpd_uri_stats_query_add(Azy_Server_Module *module, const char *path, Azy_Net_Data *data);

void httpd_uri_network_init(void);
void httpd_uri_network(void *service_data, Azy_Server_Module *module, const char *uri, Azy_Net_Data *data);

void httpd_uri_log_init(void);

char * httpd_utils_file_get(const char *file, size_t len);
char * httpd_utils_content_type(const char *file);
void httpd_utils_send(Azy_Server_Module *module, Azy_Net *net, unsigned char *s, size_t len, const char *mimetype);
#endif
