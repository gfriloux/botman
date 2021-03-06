#include "gdb.h"

Eina_Bool
botman_dumps_ls_filter(void *data,
                       Eio_File *handler EINA_UNUSED,
                       const Eina_File_Direct_Info *info)
{
   Module_Gdb *gdb = data;

   DBG("gdb[%p] info[%p][%s]", gdb, info, info->path);
   return EINA_TRUE;
}

void
botman_dumps_ls_main(void *data,
                     Eio_File *handler EINA_UNUSED,
                     const Eina_File_Direct_Info *info)
{
   Module_Gdb *gdb = data;

   DBG("gdb[%p] info[%p][%s]", gdb, info, info->path);

   gdb->dumps.check = eina_list_append(gdb->dumps.check, eina_stringshare_add(info->path));
}

void
botman_dumps_ls_done(void *data,
                     Eio_File *handler EINA_UNUSED)
{
   Module_Gdb *gdb = data;
   Eina_List *l,
             *l2;
   char *s;

   DBG("gdb[%p]", gdb);

   EINA_LIST_FOREACH(gdb->dumps.check, l, s)
     {
        char *file;

        if (utils_dump_exist(gdb->dumps.known, s))
          continue;

        DBG("New file found : %s", s);

        file = (char *)eina_stringshare_ref(s);
        EINA_SAFETY_ON_NULL_GOTO(file, end_loop);

        gdb->dumps.known = eina_list_append(gdb->dumps.known, file);

        gdb->dumps.queue = eina_list_append(gdb->dumps.queue, file);

end_loop:
        continue;
     }

   /* Check for no more existing files here. */
   EINA_LIST_FOREACH_SAFE(gdb->dumps.known, l, l2, s)
     {
        if (utils_dump_exist(gdb->dumps.check, s))
          continue;

        DBG("Deleting coredump [%s] from known list", s);
        gdb->dumps.known = eina_list_remove(gdb->dumps.known, s);
        gdb->dumps.queue = eina_list_remove(gdb->dumps.queue, s);
        eina_stringshare_del(s);
     }

   EINA_LIST_FREE(gdb->dumps.check, s)
     eina_stringshare_del(s);
   gdb->dumps.check = NULL;

   gotham_serialize_struct_to_file(gdb->dumps.known, MODULE_GDB_SAVE, (Gotham_Serialization_Function)Array_string_to_azy_value);

   /* Process queue to send backtraces */
   backtrace_get(gdb);
}

void
botman_dumps_ls_error(void *data,
                      Eio_File *handler EINA_UNUSED,
                      int error)
{
   Module_Gdb *gdb = data;

   ERR("gdb[%p] : Failed to watch coredumps directory : %s", gdb, strerror(error));
}

Eina_Bool
botman_dumps_poll(void *data)
{
   Module_Gdb *gdb = data;
   DBG("gdb[%p]", gdb);
   DBG("gdb[%p] gdb->conf->dir[%s]", gdb, gdb->conf->dir);

   eio_file_direct_ls(gdb->conf->dir,
                      botman_dumps_ls_filter,
                      botman_dumps_ls_main,
                      botman_dumps_ls_done,
                      botman_dumps_ls_error,
                      gdb);
   return EINA_TRUE;
}
