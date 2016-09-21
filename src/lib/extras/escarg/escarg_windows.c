#include "escarg_private.h"

char *
_escarg_windows_next_delim_find(
   const char *s)
{
   char   *p1;
   char   *p2;

   p1 = strchr(s, '/');
   p2 = strchr(s, '\\');

   if (!p1) return p2;

   if (!p2) return p1;

   if ( (p2 - s) > (p1 - s) )
      return p1;

   return p2;
}
int
_escarg_windows_string_has_space(
   const char *s,
   size_t      l)
{
   char *p   = (char*)s;
   char *m   = (char*)s + l;

   while (p < m) if (*(p++) == ' ') return 1;

   return 0;
}
char *
_escarg_windows_escape_string(
   const char *dir)
{
   size_t  l;
   int     r;
   char   *s;
   char   *p;
   char   *pb;
   char   *pe;

   l = strlen(dir);
   s = calloc(1, l * 2);
   EINA_SAFETY_ON_NULL_RETURN_VAL(s, NULL);

   p  = s;
   pb = _escarg_windows_next_delim_find(dir);
   EINA_SAFETY_ON_NULL_GOTO(pb, free_s);

   if (pb != dir)
     {
        strncpy(s, dir, pb - dir);
        p += pb - dir;
        goto first_run;
     }

   while (1)
     {
        size_t ld;

        pb = _escarg_windows_next_delim_find(pb);
        EINA_SAFETY_ON_NULL_GOTO(pb, free_s);

first_run:
        pe     = _escarg_windows_next_delim_find(pb + 1);
        ld     = pe ? pe - pb : (dir + l) - (pb);
        *(p++) = *pb;

        r = _escarg_windows_string_has_space(pb, ld);
        if (r)
          {
             if (*(p - 1) == '\\')
                *(p++) = '\\';
             *(p++) = '"';
          }

        strncpy(p, pb + 1, ld - 1);
        p += ld - 1;

        if (r) *(p++) = '"';
        if (!pe) break;

        *(p) = *pe;
        pb   = pe;
        if (*(pb + 1) == 0) break;
     }

   return s;

free_s:
   free(s);
   return NULL;
}
char *
_escarg_windows_escape(
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

           return _escarg_windows_escape_string(s);
        }
      case ESCARG_TYPE_CHAR:
        {
           char *c = (char*)data;
           return escarg_utils_strdupf("%c", *c);
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
escarg_windows(
   const char *fmt,
   va_list     args)
{
   return escarg_utils_escape(_escarg_windows_escape, fmt, args);
}
