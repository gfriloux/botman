#include <Eina.h>
#include <Ecore.h>
#include <Eio.h>
#include <Gotham.h>

#include "Module_Common_Azy.h"

extern int _gdb_log_dom;

typedef struct _Module_Gdb
{
   Gotham *gotham;

   Module_Gdb_Conf *conf;

   struct
   {
      Ecore_Timer *poll;
      Eina_List *known,
                *check,
                *queue;
   } dumps;

   Eina_Bool (*access_allowed)(Gotham_Module_Command *, Gotham_Citizen *);
} Module_Gdb;

#define AUTH(_a, _b, _c)                                                       \
{                                                                              \
   if ((_a->access_allowed) && (!_a->access_allowed(_b, _c)))                  \
     {                                                                         \
        ERR("%s is not autorized", _c->jid);                                   \
        Eina_Strbuf *buf = eina_strbuf_new();                                  \
        eina_strbuf_append(buf, "Access denied");                              \
        gotham_command_json_answer(".gdb", "", EINA_FALSE,                     \
                           buf, _a->gotham, _c, EINA_FALSE);                   \
        eina_strbuf_free(buf);                                                 \
        return;                                                                 \
     }                                                                         \
}
/* "debug" */

#define MODULE_GDB_CONF SYSCONF_DIR"/gotham/modules.conf.d/gdb.conf"
#define MODULE_GDB_CMD  DATA_DIR"/gotham/modules.d/gdb/command"
#define MODULE_GDB_SAVE DATA_DIR"/gotham/modules.d/gdb/gdb.save"

#define CRI(...) EINA_LOG_DOM_CRIT(_gdb_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_gdb_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_gdb_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_gdb_log_dom, __VA_ARGS__)

void backtrace_get(void *data);
Eina_Bool backtrace_new(Module_Gdb *gdb, const char *coredump, const char *jid, Eina_Bool report);

void botman_delete_send(void *data, Gotham_Citizen_Command *command);
Eina_Bool botman_dumps_poll(void *data);
void botman_fetch_send(void *data, Gotham_Citizen_Command *command);
void botman_list_send(void *data, Gotham_Citizen_Command *command);

Eina_Bool utils_dump_exist(Eina_List *list, char *s);
char * utils_coredump_name_extract(const char *path);
char * utils_dupf(const char *s, ...);
char * utils_coredump_report_format(char *coredump, size_t size, time_t date);
