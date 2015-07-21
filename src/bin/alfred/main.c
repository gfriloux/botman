#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "alfred.h"

static const Ecore_Getopt optdesc = {
   "alfred",
   NULL,
   PACKAGE_VERSION,
   "(C) ASP64",
   "All rights reserved",
   "  _______ __  ___               __ \n"
   " |   _   |  .'  _.----.-----.--|  |\n"
   " |.  1   |  |   _|   _|  -__|  _  |\n"
   " |.  _   |__|__| |__| |_____|_____|\n"
   " |:  |   |                         \n"
   " |::.|:. |                         \n"
   " `--- ---'                         \n"
   "Alfred is a botman assistant\n",
   0,
   {
      ECORE_GETOPT_STORE_TRUE('d', "debug", "Runs alfred in debug mode."),
      ECORE_GETOPT_LICENSE('L', "licence"),
      ECORE_GETOPT_COPYRIGHT('C', "copyright"),
      ECORE_GETOPT_VERSION('V', "version"),
      ECORE_GETOPT_HELP('h', "help"),
      ECORE_GETOPT_SENTINEL
   }
};

int main(int argc, const char **argv) {
   Eina_Bool opt_quit  = EINA_FALSE,
             opt_debug = EINA_FALSE;
   Gotham *gotham;

   eina_init();
   gotham_init();

   Ecore_Getopt_Value values[] = {
      ECORE_GETOPT_VALUE_BOOL(opt_debug),
      ECORE_GETOPT_VALUE_BOOL(opt_quit),
      ECORE_GETOPT_VALUE_BOOL(opt_quit),
      ECORE_GETOPT_VALUE_BOOL(opt_quit),
      ECORE_GETOPT_VALUE_BOOL(opt_quit),
      ECORE_GETOPT_VALUE_NONE
   };

   alfred_log_dom = eina_log_domain_register("alfred", EINA_COLOR_GREEN);

   ecore_app_args_set(argc, argv);
   if (ecore_getopt_parse(&optdesc, values, argc, (char **)argv) < 0)
     {
        CRI("Failed to parse args");
        gotham_shutdown();
        return EXIT_FAILURE;
     }

   if (opt_quit)
     {
        gotham_shutdown();
        return EXIT_FAILURE;
     }

   eina_log_domain_level_set("alfred",
                             opt_debug ? EINA_LOG_LEVEL_DBG :
                             EINA_LOG_LEVEL_INFO);

new_gotham:
   gotham = gotham_new(GOTHAM_CITIZEN_TYPE_ALFRED,
                       "/etc/alfred.conf");
   if (!gotham)
     {
        CRI("Failed to create gotham object !");
        sleep(10);
        goto new_gotham;
     }

   gotham_reconnect_set(gotham, EINA_TRUE);
   ecore_event_handler_add(GOTHAM_EVENT_CONNECT,
                           alfred_event_connect,
                           NULL);
   ecore_event_handler_add(GOTHAM_EVENT_CITIZEN_COMMAND,
                           alfred_event_citizen_command,
                           NULL);
   alfred_emoticons_load(gotham);

   ecore_main_loop_begin();

   gotham_free(gotham);
   gotham_shutdown();
   return EXIT_SUCCESS;
}

#undef ERR
#undef NFO
#undef DBG
#undef CRI
