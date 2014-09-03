#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <cJSON.h>

/**
 * @addtogroup Gotham_Module_Save
 * @{
 */

/**
 * @brief Get a var as number from json object.
 * @param json cJSON to read
 * @param varname Var to search for
 * @return Integer value if found, -1 otherwise
 */
int
utils_json_number_get(cJSON *json, const char *varname)
{
   cJSON *var;

   EINA_SAFETY_ON_NULL_RETURN_VAL(json, -1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(varname, -1);

   var = cJSON_GetObjectItem(json, varname);
   if (!var)
     return -1;

   if (var->type != cJSON_Number)
     return -1;

   return var->valueint;
}

/**
 * @brief Get A var as string from json object.
 * @param json cJSON to read
 * @param varname Var to search for
 * @return const char * value if found, NULL otherwise
 */
const char *
utils_json_string_get(cJSON *json, const char *varname)
{
   cJSON *var;

   EINA_SAFETY_ON_NULL_RETURN_VAL(json, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(varname, NULL);

   var = cJSON_GetObjectItem(json, varname);
   if (!var)
     return NULL;

   if (var->type != cJSON_String)
     return NULL;

   return var->valuestring;
}

/**
 * @brief Check is char is a number.
 * @param c char to check
 * @return EINA_TRUE if it's a number, EINA_FALSE otherwise
 */
Eina_Bool
utils_isnumber(char c)
{
   if (c < 48  || c > 57) return EINA_FALSE;
   return EINA_TRUE;
}

/**
 * @}
 */
