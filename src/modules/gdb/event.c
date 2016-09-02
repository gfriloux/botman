#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gdb.h"

Eina_Bool
event_connect(void *data,
              int type EINA_UNUSED,
              void *ev EINA_UNUSED)
{
   Module_Gdb *gdb = data;

   botman_dumps_poll(gdb);
   return EINA_TRUE;
}
