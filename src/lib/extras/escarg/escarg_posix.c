#include "escarg_private.h"

char *
_escarg_posix_escape_string(
   const char *s)
{
   char        *e;
   char        *p;
   char        *n;
   size_t       bloc     = 16;
   size_t       alloced  = 0;
   size_t       used     = 0;
   unsigned int i;

   e = malloc(bloc);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   alloced = bloc;

   p = e;

   *p = '\'';
   p++;

   for (i = 0; s[i]; i++)
     {
        if (s[i] == '\'')
          {
             strcpy(p, "'\\''");
             p    += 4;
             used += 4;
          }
        else
          {
             *p = s[i];
             p++;
             used++;
          }

        if ( alloced <= (used + 5) )
          {
             n = realloc(e, alloced + bloc);
             EINA_SAFETY_ON_NULL_GOTO(n, realloc_fail);
             alloced += bloc;
             p        = n + (p - e);
             e        = n;
             continue;

realloc_fail:
             free(e);
             return NULL;
          }
     }

   *p = '\'';
   p++;
   *p = '\0';
   return e;
}
char *
_escarg_posix_escape(
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
           return _escarg_posix_escape_string(s);
        }
      case ESCARG_TYPE_CHAR:
        {
           char *c = (char*)data;
           if (*c == '\'') return strdup("\\'");

           return escarg_utils_strdupf("'%c'", *c);
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
escarg_posix(
   const char *fmt,
   va_list     args)
{
   return escarg_utils_escape(_escarg_posix_escape, fmt, args);
}
