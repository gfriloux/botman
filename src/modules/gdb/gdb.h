#include <Eina.h>
#include <Ecore.h>
#include <Eio.h>
#include <Gotham.h>

extern int _gdb_log_dom;

typedef struct _Module_Gdb
{
   Gotham *gotham;

   struct
   {
      const char *dir;
      Ecore_Timer *poll;
      Eina_List *known,
                *check,
                *queue;
   } dumps;
} Module_Gdb;

#define MODULE_GDB_CONF SYSCONF_DIR"/gotham/modules.conf.d/gdb.conf"
#define MODULE_GDB_CMD  DATA_DIR"/gotham/modules.d/gdb/command"
#define MODULE_GDB_SAVE DATA_DIR"/gotham/modules.d/gdb/gdb.save"

#define CRI(...) EINA_LOG_DOM_CRIT(_gdb_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_gdb_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_gdb_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_gdb_log_dom, __VA_ARGS__)

void backtrace_get(void *data);
Eina_Bool botman_dumps_poll(void *data);
void conf_load(Module_Gdb *gdb);
void conf_backup_load(Module_Gdb *gdb);
void conf_backup_save(Module_Gdb *gdb);
void module_json_answer(const char *cmd, const char *params, Eina_Bool status, Eina_Strbuf *content, Gotham *gotham, Gotham_Citizen *citizen, Eina_Bool send_to_alfred);
Eina_Bool utils_dump_exist(Eina_List *list, char *s);
char * utils_coredump_name_extract(const char *path);
char * utils_dupf(const char *s, ...);
