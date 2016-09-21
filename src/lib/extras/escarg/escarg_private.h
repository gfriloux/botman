#include <Escarg.h>

#include <Eina.h>

extern int _escarg_log_dom_global;

#define ERR(...) EINA_LOG_DOM_ERR(_escarg_log_dom_global, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_escarg_log_dom_global, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_escarg_log_dom_global, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_escarg_log_dom_global, __VA_ARGS__)
#define CRI(...) EINA_LOG_DOM_CRIT(_escarg_log_dom_global, __VA_ARGS__)

typedef enum _Escarg_Type
{
   ESCARG_TYPE_DOUBLE,
   ESCARG_TYPE_UNSIGNED_LONG_LONG_INT,
   ESCARG_TYPE_UNSIGNED_LONG_INT,
   ESCARG_TYPE_UNSIGNED_INT,
   ESCARG_TYPE_LONG_LONG_INT,
   ESCARG_TYPE_LONG_INT,
   ESCARG_TYPE_INT,
   ESCARG_TYPE_STRING,
   ESCARG_TYPE_CHAR,
} Escarg_Type;

typedef char * (*Escaping_Function)(Escarg_Type type, void *data);

char * escarg_bash(const char *fmt, va_list args);
char * escarg_windows(const char *fmt, va_list args);
char * escarg_posix(const char *fmt, va_list args);
char * escarg_utils_escape(Escaping_Function func, const char *fmt, va_list args);
char * escarg_utils_strdupf(const char *s, ...);
