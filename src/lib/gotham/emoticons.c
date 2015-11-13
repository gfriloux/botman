#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gotham_private.h"

/**
 * @addtogroup Libgotham_Emoticons_Functions
 * @{
 */

/**
 * Add an emoticon to the gotham's emoticon list
 * Emoticons will replace some keywords in sent messages
 * @param gotham Gotham structure
 * @param file Emoticon image to add
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool gotham_emoticons_custom_add(Gotham *gotham, const char *file)
{
   char **dirs,
        *filename,
        *type,
        *p;
   unsigned int dirs_nb,
                filename_len;
   Eina_Bool ret;

   dirs = eina_str_split_full(file, "/", 42, &dirs_nb);
   if (!dirs)
     return EINA_FALSE;

   p = strrchr(dirs[dirs_nb-1], '.');
   if (!p)
     return EINA_FALSE;


   filename_len = p - dirs[dirs_nb-1];
   filename = calloc(1, filename_len + 1);
   strncpy(filename, dirs[dirs_nb-1], filename_len);

   type = calloc(1, strlen(p+1) + 6 + 1);
   sprintf(type, "image/%s", p+1);
   ret = shotgun_emoticon_custom_add(gotham->shotgun,
                                     file,
                                     filename,
                                     type);
   free(dirs[0]);
   free(dirs);
   free(filename);
   free(type);
   return ret;
}

/**
 * @}
 */
