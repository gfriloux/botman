#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>

#include "rewrite.h"

/**
 * @addtogroup Gotham_Module_Rewrite
 * @{
 */

/**
 * @brief Escape a string.
 * @param str String to escape
 * @return escaped string
 */
char *
utils_escape(const char *str)
{
   char *s2, *d;
   const char *s;

   if (!str)
      return NULL;

   s2 = malloc((strlen(str) * 2) + 1);
   if (!s2)
      return NULL;

   for (s = str, d = s2; *s != 0; s++, d++)
     {
        if (*s == '\"')
          {
             *d = '\\';
             d++;
          }

        *d = *s;
     }
   *d = 0;
   return s2;
}

/**
 * @brief Duplicate a string with arguments.
 * @param s Format string to dupe.
 * @param ... arguments
 * @return duplicated string
 */
char *
utils_strdupargs(const char *s, ...)
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

/**
 * @brief Rewrite function.
 * Turn old string into rewrited one, using a system call.
 * @param rule Module_Rewrite_Rule rule to use
 * @param s String to rewrite
 * @return const char * rewrited string.
 */
const char *
utils_rewrite(Module_Rewrite_Conf_Rule *rule, const char *s)
{
   char *s_esc,
        *cmd;
   char out[1024];
   FILE *f;
   size_t nb;
   int ret;
   DBG("rule[%p]=%s s[%p]=%s", rule, rule->name, s, s);

   s_esc = utils_escape(s);
   cmd = utils_strdupargs("echo \"%s\" | %s", s_esc, rule->rule);
   free(s_esc);
   memset(out, 0, sizeof(out));
   f = popen(cmd, "r");
   if (!f)
     return NULL;

   nb = fread(out, 1, sizeof(out)-1, f);
   ret = pclose(f);
   if ((!nb) || (nb == 1))
     return NULL;
   out[nb-1] = 0;
   free(cmd);

   if( WEXITSTATUS(ret) != 0 )
     return NULL;

   return strdup(out);
}

/**
 * @}
 */
