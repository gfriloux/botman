#include "escarg_private.h"

char *
_escarg_bash_escape_string(
   const char *s)
{
   char *e;
   char *p;
   unsigned int i;

   e = calloc(1, strlen(s) * 2 + 3 + 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   p = e;

   strcpy(p, "$'");
   p += 2;

   for (i = 0; s[i]; i++)
     {
        if (s[i] == '\'')
          {
             *p = '\\';
             p++;
          }

        *p = s[i];
        p++;
     }

   *p = '\'';
   return e;
}
char *
_escarg_bash_escape(
   Escarg_Type type,
   void       *data)
{
   switch (type)
     {
      case ESCARG_TYPE_DOUBLE:
        {
           double *d = (double*)data;
           return escarg_utils_strdupf("%f", *d);
        }
      case ESCARG_TYPE_UNSIGNED_LONG_LONG_INT:
        {
           unsigned long long int *i = (unsigned long long int*)data;
           return escarg_utils_strdupf("%llu", *i);
        }
      case ESCARG_TYPE_UNSIGNED_LONG_INT:
        {
           unsigned long int *i = (unsigned long int*)data;
           return escarg_utils_strdupf("%lu", *i);
        }
      case ESCARG_TYPE_UNSIGNED_INT:
        {
           unsigned int *i = (unsigned int*)data;
           return escarg_utils_strdupf("%u", *i);
        }
      case ESCARG_TYPE_LONG_LONG_INT:
        {
           long long int *i = (long long int*)data;
           return escarg_utils_strdupf("%lli", *i);
        }
      case ESCARG_TYPE_LONG_INT:
        {
           long int *i = (long int*)data;
           return escarg_utils_strdupf("%li", *i);
        }
      case ESCARG_TYPE_INT:
        {
           int *i = (int*)data;
           return escarg_utils_strdupf("%i", *i);
        }
      case ESCARG_TYPE_STRING:
        {
           char *s = (char*)data;
           return _escarg_bash_escape_string(s);
        }
      case ESCARG_TYPE_CHAR:
        {
           char *c = (char*)data;
           if (*c == '\'') return strdup("$'\\''");

           return escarg_utils_strdupf("$'%c'", *c);
        }
      default:
        {
           ERR("Unknown type passed");
           return NULL;
        }
     }

   return NULL;
}
char *
escarg_bash(
   const char *fmt,
   va_list     args)
{
   return escarg_utils_escape(_escarg_bash_escape, fmt, args);
}
