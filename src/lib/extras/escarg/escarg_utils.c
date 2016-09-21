#define _GNU_SOURCE
#include <stdio.h>

#include "escarg_private.h"

char *
escarg_utils_escape(
   Escaping_Function func,
   const char       *fmt,
   va_list           args)
{
   Eina_Strbuf *buf;
   const char  *p, *pp;
   char        *ret = NULL;
   size_t       fmtlen;
   Eina_Bool    r;

   buf    = eina_strbuf_new();
   fmtlen = strlen(fmt);
   pp     = strchr(fmt, '%');
   if (!pp) pp = fmt + fmtlen;
   for ( p = fmt; p && *p; pp = strchr(p, '%') )
     {
        Eina_Bool              l  = EINA_FALSE;
        Eina_Bool              ll = EINA_FALSE;
        long long int          i;
        unsigned long long int u;
        double                 d;
        char                  *s;
        char                  *esc;

        if (!pp) pp = fmt + fmtlen;
        if (pp - p)
           EINA_SAFETY_ON_FALSE_GOTO(eina_strbuf_append_length(buf, p, pp - p), err);
        if (*pp != '%') break; /* no more fmt strings */

top:
        esc = NULL;

        switch (pp[1])
          {
           case 0:
              ERR("Invalid format string!");
              goto err;

           case 'l':
              if (!l)
                 l = EINA_TRUE;
              else if (!ll)
                 ll = EINA_TRUE;
              else
                {
                   ERR("Invalid format string!");
                   goto err;
                }
              pp++;
              goto top;

           case 'f':
              if (ll)
                {
                   ERR("Invalid format string!");
                   goto err;
                }
              d   = va_arg(args, double);
              esc = func(ESCARG_TYPE_DOUBLE, &d);
              break;

           case 'i':
           case 'd':
              if (l && ll)
                {
                   i   = va_arg(args, long long int);
                   esc = func(ESCARG_TYPE_LONG_LONG_INT, &i);
                }
              else if (l)
                {
                   i   = va_arg(args, long int);
                   esc = func(ESCARG_TYPE_LONG_INT, &i);
                }
              else
                {
                   i   = va_arg(args, int);
                   esc = func(ESCARG_TYPE_INT, &i);
                }
              break;

           case 'u':
              if (l && ll)
                {
                   u   = va_arg(args, unsigned long long int);
                   esc = func(ESCARG_TYPE_UNSIGNED_LONG_LONG_INT, &u);
                }
              else if (l)
                {
                   u   = va_arg(args, unsigned long int);
                   esc = func(ESCARG_TYPE_UNSIGNED_LONG_INT, &u);
                }
              else
                {
                   u   = va_arg(args, unsigned int);
                   esc = func(ESCARG_TYPE_UNSIGNED_INT, &u);
                }
              break;

           case 's':
              if (l)
                {
                   ERR("Invalid format string!");
                   goto err;
                }
              s = va_arg(args, char *);
              if (!s) break;

              esc = func(ESCARG_TYPE_STRING, s);
              break;

           case 'c':
              if (l)
                {
                   ERR("Invalid format string!");
                   goto err;
                }
              {
                 char c;

                 c   = va_arg(args, int);
                 esc = func(ESCARG_TYPE_CHAR, &c);
              }
              break;

           case '%':
              r = eina_strbuf_append_char(buf, '%');
              EINA_SAFETY_ON_FALSE_GOTO(r, err);
              goto end_loop;

           default:
              ERR("Unsupported format string: '%s'!", pp);
              goto err;
          }

        EINA_SAFETY_ON_NULL_GOTO(esc, err);
        r = eina_strbuf_append(buf, esc);
        free(esc);
        EINA_SAFETY_ON_FALSE_GOTO(r, err);

end_loop:
        p = pp + ( (pp[1]) ? 2 : 1 );
     }
   ret = eina_strbuf_string_steal(buf);
err:
   eina_strbuf_free(buf);
   return ret;
}

char *
escarg_utils_strdupf(const char *s,
                     ...)
{
   va_list args;
   int     len;
   char   *str;

   va_start(args, s);
   len = vasprintf(&str, s, args);
   va_end(args);

   EINA_SAFETY_ON_TRUE_RETURN_VAL(len == -1, NULL);
   return str;
}
