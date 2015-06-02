#include "gdb.h"

typedef struct _Backtrace
{
   Module_Gdb *gdb;

   Eina_Array *lines;
   char *coredump;

   struct
   {
      Ecore_Event_Handler *data,
                          *del,
                          *error;
   } ev;
} Backtrace;

void
_backtrace_free(Backtrace *b)
{
   char *s;
   Eina_Array_Iterator it;
   unsigned int i;

   ecore_event_handler_del(b->ev.data);
   ecore_event_handler_del(b->ev.del);
   ecore_event_handler_del(b->ev.error);

   free(b->coredump);

   EINA_ARRAY_ITER_NEXT(b->lines, i, s, it)
     free(s);

   free(b);
}

Eina_Bool
_backtrace_data(void *data,
                int type EINA_UNUSED,
                void *event)
{
   Backtrace *b = data;
   Ecore_Exe_Event_Data *d = (Ecore_Exe_Event_Data *)event;
   unsigned int i;

   DBG("b[%p]", b);

   for (i = 0; d->lines[i].line; i++)
     {
        eina_array_push(b->lines, strdup(d->lines[i].line));
     }
   return EINA_TRUE;
}

Eina_Bool
_backtrace_del(void *data,
               int type EINA_UNUSED,
               void *event EINA_UNUSED)
{
   Backtrace *b = data;
   unsigned int i;
   char *s;
   Eina_Array_Iterator it;
   Eina_Strbuf *buf;

   DBG("End of backtrace analysis");

   buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_GOTO(buf, free_b);

   eina_strbuf_append_printf(buf, "Coredump file : %s\n", b->coredump);

   EINA_ARRAY_ITER_NEXT(b->lines, i, s, it)
     {
        DBG("s[%s]", s);
        eina_strbuf_append(buf, s);
        eina_strbuf_append_char(buf, '\n');
     }

   module_json_answer(".gdb", "backtrace", EINA_TRUE, buf,
                      b->gdb->gotham, b->gdb->gotham->alfred, EINA_FALSE);

   eina_strbuf_free(buf);
free_b:
   _backtrace_free(b);
   conf_backup_save(b->gdb);
   backtrace_get(b->gdb);
   return EINA_TRUE;
}

Eina_Bool
_backtrace_error(void *data,
                 int type EINA_UNUSED,
                 void *event EINA_UNUSED)
{
   Backtrace *b = data;

   ERR("b[%p] : Failed to fetch backtrace", b);

   _backtrace_free(b);
   return EINA_TRUE;
}

Backtrace *
_backtrace_new(Module_Gdb *gdb,
               const char *coredump)
{
   Backtrace *b;
   char *app,
        *cmd;

   app = utils_coredump_name_extract(coredump);
   EINA_SAFETY_ON_NULL_RETURN_VAL(app, NULL);

   cmd = utils_dupf("/usr/bin/gdb -batch -x "MODULE_GDB_CMD" "
                    "/usr/local/bin/%s -c %s", app, coredump);
   EINA_SAFETY_ON_NULL_GOTO(cmd, free_app);

   DBG("cmd[%s]", cmd);

   b = calloc(1, sizeof(Backtrace));
   EINA_SAFETY_ON_NULL_GOTO(b, free_cmd);

   b->gdb = gdb;

   b->coredump = strdup(coredump);
   EINA_SAFETY_ON_NULL_GOTO(b->coredump, free_b);

#define _EV(_a, _b, _c, _d) _a = ecore_event_handler_add(ECORE_EXE_EVENT_##_b, _c, _d)
   _EV(b->ev.data, DATA, _backtrace_data, b);
   EINA_SAFETY_ON_NULL_GOTO(b->ev.data, free_coredump);

   _EV(b->ev.del, DEL, _backtrace_del, b);
   EINA_SAFETY_ON_NULL_GOTO(b->ev.del, del_data);

   _EV(b->ev.error, ERROR, _backtrace_error, b);
   EINA_SAFETY_ON_NULL_GOTO(b->ev.error, del_del);
#undef _EV

   b->lines = eina_array_new(5);
   EINA_SAFETY_ON_NULL_GOTO(b->lines, del_error);

   ecore_exe_pipe_run(cmd,
                      ECORE_EXE_PIPE_READ_LINE_BUFFERED |
                      ECORE_EXE_PIPE_READ, b);
   free(cmd);
   free(app);

   return b;

del_error:
   ecore_event_handler_del(b->ev.error);
del_del:
   ecore_event_handler_del(b->ev.del);
del_data:
   ecore_event_handler_del(b->ev.data);
free_coredump:
   free(b->coredump);
free_b:
   free(b);
free_cmd:
   free(cmd);
free_app:
   free(app);
   return NULL;
}

void
backtrace_get(void *data)
{
   Module_Gdb *gdb = data;
   char *s;

   if (!gdb->dumps.queue) return;

   s = eina_list_nth(gdb->dumps.queue, 0);
   if (!s) return;

   gdb->dumps.queue = eina_list_remove(gdb->dumps.queue, s);

   _backtrace_new(gdb, s);
}
