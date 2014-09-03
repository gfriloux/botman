#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Gotham.h>

#include "help.h"

/**
 * @addtogroup Gotham_Module_Help
 * @{
 */

/**
 * Define module name
 */
static const char *name = "Help";

/**
 * @brief Help Module init function.
 * Init needed libraries (eina), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   _module_help_log_dom = eina_log_domain_register("module_help",
                                                   EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Help Module register function.
 * Allocate module object and register functions.
 * @param gotham Gotham object
 * @return Help module structure
 */
void *
module_register(Gotham *gotham)
{
   Module_Help *help;

   if ((gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN))
     {
        NFO("Cant load for this gotham type");
        return NULL;
     }

   help = calloc(1, sizeof(Module_Help));
   if (!help)
     {
        ERR("Faild to alloc");
        return NULL;
     }
   help->gotham = gotham;

   gotham_modules_command_add("help", ".help",
                              "[.help] - "
                              "This command allows you to list all the "
                              "commands registered by modules.");
   return help;
}

/**
 * @brief Help Module unregister function.
 * Remove functions from list and free Module_Help structure
 * @param data Module_Help structure
 */
void
module_unregister(void *data)
{
   Module_Help *help = data;
   EINA_SAFETY_ON_NULL_RETURN(help);
   gotham_modules_command_del("help", ".help");
   free(help);
}

/**
 * @brief Help Module shutdown function.
 * Unregister from eina_log and shutdown eina
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_help_log_dom);
   eina_shutdown();
}

/**
 * @}
 */
