#include "gdb.h"

Eina_Bool
utils_dump_exist(Eina_List *list, char *s)
{
   Eina_List *l;
   char *s2;

   EINA_LIST_FOREACH(list, l, s2)
     if (!strcmp(s, s2)) return EINA_TRUE;

   return EINA_FALSE;
}

char *
utils_coredump_name_extract(const char *path)
{
   char *b,
        *e,
        *s;

   b = strrchr(path, '/');
   EINA_SAFETY_ON_NULL_RETURN_VAL(b, NULL);

   b += 1;
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!*b, NULL);

   e = strchr(b, '.');
   if (!e) e = (char *)path + strlen(path);

   s = calloc(1, e - b + 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(s, NULL);

   strncpy(s, b, e - b);
   DBG("app[%s]", s);
   return s;
}

char *
utils_dupf(const char *s, ...)
{
   va_list args;
   int len;
   char *str;

   va_start(args, s);
   len = vasprintf(&str, s, args);
   va_end(args);

   EINA_SAFETY_ON_TRUE_RETURN_VAL(len == -1, NULL);
   return str;
}
