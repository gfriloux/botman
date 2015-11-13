#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include "gotham_private.h"

/**
 * @addtogroup Libgotham_Modules_Functions
 * @{
 */

/**
 * Load and parse a json file into a JSON structure
 * @param file File to load
 * @return cJSON loaded structure
 */
cJSON *
gotham_modules_conf_load(const char *file)
{
   Eina_File *fh;
   const char *data;
   cJSON *json;

   fh = eina_file_open(file, EINA_FALSE);
   if (!fh)
     {
        NFO("Unable to open file “%s”", file);
        return NULL;
     }

   data = (const char *)eina_file_map_all(fh, EINA_FILE_SEQUENTIAL);

   json = cJSON_Parse(data);
   eina_file_map_free(fh, (void *)data);
   eina_file_close(fh);

   if (!json)
     {
        ERR("Failed to parse configuration file “%s” !", file);
        return NULL;
     }

   return json;
}

/**
 * Save a module conf into file.
 * @param file File to write
 * @param json cJSON structure containing module conf.
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
gotham_modules_conf_save(const char *file, cJSON *json)
{
   int fd;
   ssize_t size = 0,
           offset = 0,
           nb = 0;
   char *json_text;
   char file_tmp[strlen(file)+4];

   sprintf(file_tmp, "%s.tmp", file);
   fd = open(file_tmp, O_WRONLY | O_CREAT | O_TRUNC, 0700);
   if (fd == -1)
     {
        ERR("Failed to open %s : %s", file_tmp, strerror(errno));
        return EINA_FALSE;
     }

   json_text = cJSON_Print(json);
   size = strlen(json_text);
   while(offset != size)
     {
        nb = write(fd, json_text + offset, size - offset);
        if (nb == -1)
          {
             ERR("Failed to write %s : %s", file_tmp, strerror(errno));
             free(json_text);
             close(fd);
             return EINA_FALSE;
          }

        offset += nb;
     }
   free(json_text);
   close(fd);

   if (rename(file_tmp, file))
     {
        ERR("Failed to rename %s to %s : %s", file_tmp, file, strerror(errno));
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

/**
 * @}
 */
