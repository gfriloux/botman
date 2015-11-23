#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "botman.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static const Ecore_Getopt optdesc = {
   "botman",
   NULL,
   PACKAGE_VERSION,
   "(C) ASP64",
   "All rights reserved",
   " _______       __                        \n"
   " |   _   .-----|  |_.--------.---.-.-----.\n"
   " |.  1   |  _  |   _|        |  _  |     |\n"
   " |.  _   |_____|____|__|__|__|___._|__|__|\n"
   " |:  1    \\                               \n"
   " |::.. .  /                               \n"
   " `-------'                                \n"
   "                                          \n"
   "Botman is a XMPP bot\n",
   0,
 {
      ECORE_GETOPT_STORE_TRUE('d', "debug", "Runs botman in debug mode."),
      ECORE_GETOPT_STORE_STR('l', "log", "Log messages into given file"),
      ECORE_GETOPT_LICENSE('L', "license"),
      ECORE_GETOPT_COPYRIGHT('C', "copyright"),
      ECORE_GETOPT_VERSION('V', "version"),
      ECORE_GETOPT_HELP('h', "help"),
      ECORE_GETOPT_SENTINEL
 }
};

char *_pid_file = "/var/run/botman.pid";

#ifdef _WIN32
void WINAPI
ServCtrlHandler(DWORD SCCode)
{
    switch(SCCode)
     {
      case SERVICE_CONTROL_CONTINUE:
          DBG("SERVICE_CONTROL_CONTINUE");
          ServiceStatus.dwCurrentState = SERVICE_RUNNING;
          break;
      case SERVICE_CONTROL_PAUSE:
          DBG("SERVICE_CONTROL_PAUSE");
          ServiceStatus.dwCurrentState = SERVICE_PAUSED;
          break;
      case SERVICE_CONTROL_STOP:
          DBG("SERVICE_CONTROL_STOP");
          ServiceStatus.dwCurrentState = SERVICE_STOPPED;
          ServiceStatus.dwWin32ExitCode = 0;
          ServiceStatus.dwCheckPoint = 0;
          ServiceStatus.dwWaitHint = 0;
          ServiceStarted=FALSE;
          SetServiceStatus(hServiceStatus, &ServiceStatus);
          ecore_main_loop_quit();
          return;
     }
   SetServiceStatus(hServiceStatus, &ServiceStatus);
}

void WINAPI
service_main(DWORD dwArgc,
             LPTSTR *lpszArgv)
{
   BOOL r;

   ZeroMemory(&ServiceStatus, sizeof(SERVICE_STATUS));
   ServiceStatus.dwServiceType = SERVICE_WIN32;
   ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
   ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

   DBG("Registering service");
   hServiceStatus = RegisterServiceCtrlHandler("Botman", ServCtrlHandler);
   if (!hServiceStatus)
     {
        ERR("RegisterServiceCtrlHandler failed (%d)", GetLastError());
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwCheckPoint = 0;
        ServiceStatus.dwWaitHint = 0;
        ServiceStatus.dwWin32ExitCode = 1;
        SetServiceStatus(hServiceStatus, &ServiceStatus);
        return;
     }

   ServiceStatus.dwCurrentState = SERVICE_RUNNING;
   ServiceStatus.dwCheckPoint = 0;
   ServiceStatus.dwWaitHint = 0;

   DBG("Set service as running");
   r = SetServiceStatus(hServiceStatus, &ServiceStatus);
   if (!r)
     {
        ERR("SetServiceStatus failed (%d)", GetLastError());
        return;
     }

   ServiceStarted = TRUE;

   ecore_main_loop_begin();
   exit(0);
}

#else
void
botman_daemonize_pid(void)
{
   int fd;
   char pid[10];
   size_t size,
          offset = 0;
   ssize_t nb;

   fd = open(_pid_file, O_WRONLY|O_CREAT|O_TRUNC, 0600);
   if (fd == -1)
     {
        ERR("Failed to create pid file : %s", strerror(errno));
        return;
     }

   sprintf(pid, "%i", getpid());
   size = strlen(pid);

   do
   {
      nb = write(fd, pid + offset, size - offset);
      if (nb == -1)
        {
           ERR("Failed to write to pid file : %s", strerror(errno));
           goto close_fd;
        }

      offset += (size_t)nb;
   } while (offset != size);

   close(fd);
   return;

close_fd:
   close(fd);
   remove(_pid_file);
}
#endif

void
_print_cb(const Eina_Log_Domain *domain,
          Eina_Log_Level level EINA_UNUSED,
          const char *file,
          const char *fnc,
          int line,
          const char *fmt,
          void *data,
          va_list args)
{
   FILE *fd = data;

   fprintf(fd, "%s:%s:%s (%d): ",
           domain->domain_str, file, fnc, line);
   vfprintf(fd, fmt, args);
   putc('\n', fd);
   fflush(fd);
}


int main(int argc, const char **argv) {
   Eina_Bool opt_quit  = EINA_FALSE,
             opt_debug = EINA_FALSE;
   Gotham *gotham;
   char *opt_log = NULL;
   FILE *fd = NULL;
#ifdef _WIN32
   SERVICE_TABLE_ENTRY Table[] =
   {
      { "Botman" , service_main },
      { NULL     , NULL         }
   };
   ServiceStarted = TRUE;
#endif


   eina_init();
   gotham_init();

   Ecore_Getopt_Value values[] = {
      ECORE_GETOPT_VALUE_BOOL(opt_debug),
      ECORE_GETOPT_VALUE_STR (opt_log),
      ECORE_GETOPT_VALUE_BOOL(opt_quit),
      ECORE_GETOPT_VALUE_BOOL(opt_quit),
      ECORE_GETOPT_VALUE_BOOL(opt_quit),
      ECORE_GETOPT_VALUE_BOOL(opt_quit),
      ECORE_GETOPT_VALUE_NONE
   };

   connected = EINA_FALSE;
   botman_log_dom = eina_log_domain_register("botman", EINA_COLOR_GREEN);

   ecore_app_args_set(argc, argv);
   if (ecore_getopt_parse(&optdesc, values, argc, (char **)argv) < 0) {
      CRI("Failed to parse args");
      gotham_shutdown();
      return EXIT_FAILURE;
   }

   if (opt_quit)
     {
        gotham_shutdown();
        return EXIT_FAILURE;
     }

   eina_log_domain_level_set("botman",
                             opt_debug ? EINA_LOG_LEVEL_DBG :
                             EINA_LOG_LEVEL_INFO);

   if (opt_log)
     {
        DBG("Using log file [%s]", opt_log);
        fd = fopen(opt_log, "w");
        if (fd) eina_log_print_cb_set(_print_cb, fd);
     }

#ifndef _WIN32
   botman_daemonize_pid();
#endif

new_gotham:
#ifdef _WIN32
   {
      char path[1024],
           conf[1024],
           *p;

      GetModuleFileName(NULL, path, sizeof(path));

      p = strrchr(path, '\\');
      sprintf(conf, "%.*s"SYSCONF_DIR"\\botman\\botman.conf", p-path, path);
      gotham = gotham_new(GOTHAM_CITIZEN_TYPE_BOTMAN, conf);
   }
#else
   gotham = gotham_new(GOTHAM_CITIZEN_TYPE_BOTMAN,
                       SYSCONF_DIR"/botman/botman.conf");
#endif
   if (!gotham)
     {
        CRI("Failed to create gotham object !");
        sleep(10);
        goto new_gotham;
     }

   gotham_reconnect_set(gotham, EINA_TRUE);

   ecore_event_handler_add(GOTHAM_EVENT_CONNECT, botman_event_connect, NULL);
   ecore_event_handler_add(GOTHAM_EVENT_DISCONNECT, botman_event_disconnect, NULL);
   ecore_event_handler_add(GOTHAM_EVENT_CITIZEN_LIST, botman_event_citizen_list, NULL);
   ecore_event_handler_add(GOTHAM_EVENT_CITIZEN_CONNECT, botman_event_citizen_connect, NULL);
   ecore_event_handler_add(GOTHAM_EVENT_CITIZEN_COMMAND, botman_event_citizen_command, NULL);

#ifdef _WIN32
   StartServiceCtrlDispatcher(Table);
   switch (GetLastError())
     {
      case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
        ERR("ERROR_FAILED_SERVICE_CONTROLLER_CONNECT");
        break;
      case ERROR_INVALID_DATA:
        ERR("ERROR_INVALID_DATA");
        break;
      case ERROR_SERVICE_ALREADY_RUNNING:
        ERR("ERROR_SERVICE_ALREADY_RUNNING");
        break;
      case 0:
        ERR("NO ERROR");
        goto free_gotham;
      default:
        ERR("Unknown error %d", GetLastError());
        break;
     }
#endif

   ecore_main_loop_begin();

#ifdef _WIN32
free_gotham:
#endif
   gotham_free(gotham);
   gotham_shutdown();
   return EXIT_SUCCESS;
}

#undef ERR
#undef NFO
#undef DBG
#undef CRI
