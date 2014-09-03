#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "spam.h"

/**
 * @addtogroup Gotham_Module_Spam
 * @{
 */

/**
 * @brief Convert a char ** array to char * string.
 * @param array Array to convert
 * @param d delimiter to use
 * @return const char * result string
 */
const char *
utils_atos(const char **array, const char *d)
{
   const char *p;
   Eina_Strbuf *b;
   unsigned int i = 0;

   DBG("array[%p]", array);
   EINA_SAFETY_ON_NULL_RETURN_VAL(array, NULL);

   b = eina_strbuf_new();
   while (array[i])
     {
        eina_strbuf_append(b, array[i++]);
        if (!array[i])
          break;
        eina_strbuf_append(b, d);
     }
   p = eina_strbuf_string_steal(b);
   eina_strbuf_free(b);

   return p;
}

/**
 * @}
 */
