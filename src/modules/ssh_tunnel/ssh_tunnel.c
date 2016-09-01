#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <cJSON.h>

#include "ssh_tunnel.h"


/**
 * @addtogroup Gotham_Module_Ssh_Tunnel
 * @{
 */

/**
 * Defines module name
 */
static const char *name = "Ssh_Tunnel";

/**
 * @brief Ssh Module init function.
 * Init needed libraries (eina, ecore), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   ecore_init();
   _module_log_dom = eina_log_domain_register("module_ssh_tunnel",
                                              EINA_COLOR_CYAN);

   DBG("void");

   return name;
}

/**
 * @brief Module register function.
 * Allocate module object, add functions and load conf.
 * Add ecore event handlers for ecore_exe calls
 * @param gotham Gotham object
 * @return Module object
 */
void *
module_register(Gotham *gotham)
{
   Module_Ssh_Tunnel *obj;

   DBG("gotham[%p]", gotham);

   if ((gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN) &&
       (gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED))
     {
        NFO("Cant load for this gotham type");
        return NULL;
     }

   obj = calloc(1, sizeof(Module_Ssh_Tunnel));
   if (!obj)
     {
        ERR("Faild to alloc");
        return NULL;
     }
   obj->gotham = gotham;

   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        conf_load(obj);
        obj->tunnel.eh_data = ecore_event_handler_add(ECORE_EXE_EVENT_DATA,
                                                      ssh_tunnel_cb_data,
                                                      obj);

        obj->tunnel.eh_end = ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
                                                     ssh_tunnel_cb_end,
                                                     obj);

        gotham_modules_command_add("ssh_tunnel", ".ssh",
                                   "[.ssh] - "
                                   "This command will show informations "
                                   "about the tunnel (opened or not, "
                                   "which port)", NULL);
        gotham_modules_command_add("ssh_tunnel", ".ssh on",
                                   "[.ssh on] - "
                                   "This command will try to open a "
                                   "tunnel and send back the port to use. ", NULL);
        gotham_modules_command_add("ssh_tunnel", ".ssh off",
                                   "[.ssh off] - "
                                   "This command will shut down the tunnel "
                                   "if it's opened.", NULL);
     }
   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     {
        conf_load(obj);
        gotham_modules_command_add("ssh_tunnel", ".ssh",
                                   "[.ssh pattern] - "
                                   "This command will show informations "
                                   "about the tunnel for the matching "
                                   "pattern", NULL);
     }

   return obj;
}

/**
 * @brief Module unregister function.
 * Remove functions from list, free all objects and then
 * free Module object
 * @param data Module object
 */
void
module_unregister(void *data)
{
#define _FREE(a) if (obj->a) free((char *)obj->a)
   Module_Ssh_Tunnel *obj = data;
   char *item;

   EINA_SAFETY_ON_NULL_RETURN(obj);

   DBG("obj[%p]", obj);

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        ecore_event_handler_del(obj->tunnel.eh_data);
        ecore_event_handler_del(obj->tunnel.eh_end);
        gotham_modules_command_del("ssh_tunnel", ".ssh");
        gotham_modules_command_del("ssh_tunnel", ".ssh on");
        gotham_modules_command_del("ssh_tunnel", ".ssh off");
     }

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     {
        EINA_LIST_FREE(obj->vars, item) free(item);
        gotham_modules_command_del("ssh_tunnel", ".ssh");
     }

   free(obj);
#undef _FREE
}

/**
 * @brief Module shutdown function.
 * Unregister from eina_log and shutdown ecore and eina
 */
void
module_shutdown(void)
{
   DBG("void");
   eina_log_domain_unregister(_module_log_dom);
   ecore_shutdown();
   eina_shutdown();
}

/**
 * @}
 */
