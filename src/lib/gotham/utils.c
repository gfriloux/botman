#include <Eina.h>
#include <cJSON.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * @addtogroup Libgotham_Utils_Functions
 * @{
 */

/**
 * Turns a char * message to a char ** command
 * Message wil be splitted on single spaces. Empty
 * "lines" will be removed from final char **
 * @param msg Message to split
 * @return const char ** splitted message
 * @todo : handle quotes-contained parameters as a single word
 */
const char **
gotham_utils_msgtocommand(const char *msg)
{
   unsigned int i,
                j,
                len = 0,
                count = 0;
   char **params,
        **command,
        *words,
        *p;

   params = eina_str_split(msg, " ", 128);

   for (i=0; params[i]; i++)
     {
        size_t params_len = strlen(params[i]);

        if (!params_len)
          continue;

        count++;

        if (len < params_len)
          len = params_len;
     }

   command = calloc(count+1, sizeof(char *));
   words = calloc(count, len + 1);

   for (i=j=0; params[i]; i++)
     {
        if (!params[i][0])
          continue;

        p = words + j * (len + 1);

        strcpy(p, params[i]);
        command[j++] = p;
     }
   free(params[0]);
   free(params);
   return (const char **)command;
}

/**
 * Turns a cJSON structure message to a char ** command
 * Each JSON's array item will be a element in returning char **
 * @param json_var cJSON content to split
 * @return const char ** splitted message
 */
const char **
gotham_utils_json_array_stringify(cJSON *json_var)
{
   char **command,
        *words,
        *p;
   unsigned int i,
                j,
                size,
                len = 0,
                count = 0;

   size = cJSON_GetArraySize(json_var);

   for (i=0; i < size; i++)
     {
        cJSON *json_content = cJSON_GetArrayItem(json_var, i);

        size_t params_len = strlen(json_content->valuestring);

        if (!params_len)
          continue;

        count++;

        if (len < params_len)
          len = params_len;
     }

   command = calloc(count + 1, sizeof(char *));
   words = calloc(count, len + 1);

   for (i=j=0; i < size; i++)
     {
        cJSON *json_content = cJSON_GetArrayItem(json_var, i);
        if (!json_content->valuestring)
          continue;

        p = words + j * (len + 1);

        strcpy(p, json_content->valuestring);
        command[j++] = p;
     }

   return (const char **)command;
}

#ifdef _WIN32
char *
gotham_utils_strndup(const char *s,
                     size_t n)
{
   char  *p;
   size_t l = strlen(s);

   if (l > n) l = n;

   p = (char*)malloc(l + 1);
   if (!p) return NULL;

   p[l] = '\0';
   return (char*)memcpy(p, s, l);
}
#endif

char *
gotham_utils_file_data_read(const char *file,
                            size_t *size)
{
   Eina_File *ef;
   char *p,
        *s;

   EINA_SAFETY_ON_NULL_RETURN_VAL(file, NULL);

   ef = eina_file_open(file, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ef,   NULL);

   p = (char*)eina_file_map_all(ef, EINA_FILE_SEQUENTIAL);
   EINA_SAFETY_ON_NULL_GOTO(p, free_ef);

   *size = eina_file_size_get(ef);

#ifdef _WIN32
   s = gotham_utils_strndup(p, *size);
#else
   s = strndup(p, *size);
#endif
   EINA_SAFETY_ON_NULL_GOTO(s, free_ef);

   eina_file_map_free(ef, (void*)p);
   eina_file_close(ef);

   return s;

free_ef:
   eina_file_close(ef);
   return NULL;
}

Eina_Bool
gotham_utils_file_data_write(const char *file,
                             const char *data,
                             size_t size)
{
   int fd;
   ssize_t nb;
   size_t  offset = 0;

   fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(fd == -1, EINA_FALSE);

#ifdef _WIN32
   _setmode(fd, _O_BINARY);
#endif

   while (offset != size)
     {
        nb = write(fd, data + offset, size - offset);
        EINA_SAFETY_ON_TRUE_GOTO(nb == -1, close_fd);

        offset += nb;
     }

   close(fd);
   return EINA_TRUE;

close_fd:
   close(fd);
   return EINA_FALSE;
}

/**
 * @}
 */
