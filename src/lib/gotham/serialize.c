#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gotham_private.h"

/**
 * @since 1.3.0
 */
void *
gotham_serialize_string_to_struct(const char *s,
                                  size_t len,
                                  Gotham_Deserialization_Function func)
{
   Azy_Content *ac;
   Eina_Bool r;
   Eina_Value *ev;
   void *data = NULL;

   ac = azy_content_new(NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ac, NULL);

   r = azy_content_deserialize_json(ac, s, len);
   EINA_SAFETY_ON_TRUE_GOTO(!r, free_ac);

   ev = azy_content_retval_get(ac);
   EINA_SAFETY_ON_NULL_GOTO(ev, free_ac);

   func(ev, &data);

free_ac:
   azy_content_free(ac);
   return data;
}

/**
 * @since 1.3.0
 */
char *
gotham_serialize_struct_to_string(void *data,
                                  Gotham_Serialization_Function func)
{
   char *s;
   Eina_Value *ev = NULL;

   ev = func(data);

   s = azy_content_serialize_json(ev);
   eina_value_free(ev);
   EINA_SAFETY_ON_NULL_RETURN_VAL(s, NULL);

   return s;
}

/**
 * @since 1.3.0
 */
void *
gotham_serialize_file_to_struct(const char *file,
                                Gotham_Deserialization_Function func)
{
   char *s;
   size_t l;

   s = gotham_utils_file_data_read(file, &l);
   EINA_SAFETY_ON_NULL_RETURN_VAL(s, NULL);

   return gotham_serialize_string_to_struct(s, l, func);
}

/**
 * @since 1.3.0
 */
Eina_Bool
gotham_serialize_struct_to_file(void *data,
                                const char *file,
                                Gotham_Serialization_Function func)
{
   char *s;
   Eina_Bool r;

   s = gotham_serialize_struct_to_string(data, func);
   EINA_SAFETY_ON_NULL_RETURN_VAL(s, EINA_FALSE);

   r = gotham_utils_file_data_write(file, s, strlen(s));
   free(s);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!r, EINA_FALSE);

   return EINA_TRUE;
}
