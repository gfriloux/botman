#define _GNU_SOURCE
#include <stdio.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdarg.h>

char *
dupf(const char *s, ...)
{
   va_list args;
   int len;
   char *str;

   va_start(args, s);
   len = vasprintf(&str, s, args);
   va_end(args);

   if (len == -1)
     return NULL;

   return str;
}
