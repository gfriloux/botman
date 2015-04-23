#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <Gotham.h>

#include "sysinfo.h"

/**
 * @addtogroup Gotham_Module_Sysinfo
 * @{
 */

/**
 * Define module name
 */
static const char *name = "Sysinfo";

/**
 * @brief Callback for freeing a eina_hash value from the obj->hw hash
 * @param data data to free
 */
static void
_hash_free_cb(void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(data);
   free(data);
}

/**
 * @brief Module init function.
 * Init needed libraries (eina, ecore), and register in eina_log_domain
 * @return Module name
 */
const char *
module_init(void)
{
   eina_init();
   ecore_init();
   _module_log_dom = eina_log_domain_register("module_sysinfo",
                                              EINA_COLOR_CYAN);
   return name;
}

/**
 * @brief Module register function.
 * Allocate module object and register functions.
 * @param gotham Gotham object
 * @return Module object
 */
void *
module_register(Gotham *gotham)
{
   Module_Sysinfo *obj;

   if ((gotham->me->type != GOTHAM_CITIZEN_TYPE_ALFRED) &&
       (gotham->me->type != GOTHAM_CITIZEN_TYPE_BOTMAN))
     {
        NFO("Cant load for this gotham type");
        return NULL;
     }

   obj = calloc(1, sizeof(Module_Sysinfo));
   if (!obj)
     {
        ERR("Faild to alloc");
        return NULL;
     }
   obj->gotham = gotham;

   obj->hw = eina_hash_string_small_new(_hash_free_cb);
   obj->commands = eina_hash_string_small_new(_hash_free_cb);
   obj->vars = eina_array_new(1);

   conf_load(obj);

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     {
        /* Gather sysinfo and store as custom vars */
        botman_sysinfo_get(obj);
        botman_commands_add(obj);
     }

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     alfred_commands_add(obj);

   return obj;
}

/**
 * @brief Module unregister function.
 * Remove functions from list and free Module object
 * @param data Module object
 */
void
module_unregister(void *data)
{
   Module_Sysinfo *obj = data;
   char *item;
   Eina_Array_Iterator iterator;
   unsigned int i;

   EINA_SAFETY_ON_NULL_RETURN(obj);

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     botman_commands_del(obj);

   if (obj->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED)
     alfred_commands_del(obj);

   EINA_ARRAY_ITER_NEXT(obj->vars, i, item, iterator)
     free(item);
   eina_array_free(obj->vars);

   eina_hash_free(obj->hw);

   free(obj);
}

/**
 * @brief Module shutdown function.
 * Unregister from eina_log and shutdown eina and ecore
 */
void
module_shutdown(void)
{
   eina_log_domain_unregister(_module_log_dom);
   ecore_shutdown();
   eina_shutdown();
}

/**
 * @}
 */
