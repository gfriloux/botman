#ifndef ESCARG_H
#define ESCARG_H
typedef enum _Escarg_Mode
{
   ESCARG_MODE_BASH,
   ESCARG_MODE_WINDOWS,
   ESCARG_MODE_POSIX
} Escarg_Mode;

int escarg_init(void);
int escarg_shutdown(void);
char * escarg(Escarg_Mode mode, const char *fmt, ...);
#endif
