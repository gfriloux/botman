#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eio.h>

#include "alfred.h"

Eina_Bool
_alfred_emoticons_filter(void *data EINA_UNUSED,
                         Eio_File *handler EINA_UNUSED,
                         const char *file EINA_UNUSED)
{
   return EINA_TRUE;
}

void
_alfred_emoticons_error(void *data EINA_UNUSED,
                        Eio_File *handler EINA_UNUSED,
                        int error)
{
   ERR("Could not list "DATA_DIR"/alfred/emoticons/ : %s", strerror(error));
}

void
_alfred_emoticons_done(void *data EINA_UNUSED,
                       Eio_File *Handler EINA_UNUSED)
{
   DBG("Loading of emoticons done");
}

void
_alfred_emoticons_list(void *data,
                       Eio_File *handler EINA_UNUSED,
                       const char *file)
{
   Gotham *gotham = data;
   DBG("Adding %s as emoticon", file);
   gotham_emoticons_custom_add(gotham, file);
}

void
alfred_emoticons_load(Gotham *gotham)
{

   eio_file_ls(DATA_DIR"/alfred/emoticons/",
               _alfred_emoticons_filter,
               _alfred_emoticons_list,
               _alfred_emoticons_done,
               _alfred_emoticons_error,
               gotham);
}
