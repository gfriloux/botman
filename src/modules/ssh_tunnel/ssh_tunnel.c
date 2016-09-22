#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>

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
   escarg_init();
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

   obj = calloc(1, sizeof(Module_Ssh_Tunnel));
   if (!obj)
     {
        ERR("Faild to alloc");
        return NULL;
     }
   DBG("obj[%p]", obj);
   obj->gotham = gotham;


#ifdef _WIN32
   {
      char path[1024],
           conf[1024],
           *p;

      GetModuleFileName(NULL, path, sizeof(path));

      p = strrchr(path, '\\');
      sprintf(conf, "%.*s"MODULE_CONF, p-path, path);
      obj->conffile = strdup(conf);

      obj->conf = gotham_serialize_file_to_struct(obj->conffile,
                                                 (Gotham_Deserialization_Function)azy_value_to_Module_Ssh_Tunnel_Conf);
   }
#else
   obj->conf = gotham_serialize_file_to_struct(MODULE_CONF,
                                               (Gotham_Deserialization_Function)azy_value_to_Module_Ssh_Tunnel_Conf);
#endif

   if (!obj->conf->forwarded_port) obj->conf->forwarded_port = 22;

   if (gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     {
        gotham_modules_command_add("ssh_tunnel", ".ssh",
                                   "[.ssh pattern] - "
                                   "This command will show informations "
                                   "about the tunnel for the matching "
                                   "pattern",
                                   alfred_command_tunnels_show);
        return obj;
     }

   obj->tunnel.eh_data = ecore_event_handler_add(ECORE_EXE_EVENT_DATA,
                                                 ssh_tunnel_cb_data, obj);

   obj->tunnel.eh_end = ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
                                                ssh_tunnel_cb_end, obj);

   gotham_modules_command_add("ssh_tunnel", ".ssh",
                              "[.ssh] - "
                              "This command will show informations "
                              "about the tunnel (opened or not, "
                              "which port)",
                              ssh_tunnel_get);
   gotham_modules_command_add("ssh_tunnel", ".ssh on",
                              "[.ssh on] - "
                              "This command will try to open a "
                              "tunnel and send back the port to use. ",
                              ssh_tunnel_on);
   gotham_modules_command_add("ssh_tunnel", ".ssh off",
                              "[.ssh off] - "
                              "This command will shut down the tunnel "
                              "if it's opened.",
                              ssh_tunnel_off);
   DBG("Returning obj[%p]", obj);
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

   EINA_SAFETY_ON_NULL_RETURN(obj);

   DBG("obj[%p]", obj);

#ifdef _WIN32
   free((char *)obj->conffile);
#endif

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
        gotham_modules_command_del("ssh_tunnel", ".ssh");
     }
   Module_Ssh_Tunnel_Conf_free(obj->conf);

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
   escarg_shutdown();
   eina_log_domain_unregister(_module_log_dom);
   ecore_shutdown();
   eina_shutdown();
}

/**
 * @}
 */
