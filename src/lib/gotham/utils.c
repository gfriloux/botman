#include <Eina.h>
#include <cJSON.h>

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

/**
 * @}
 */
