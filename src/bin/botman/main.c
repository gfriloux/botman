#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "botman.h"

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
      ECORE_GETOPT_LICENSE('L', "license"),
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

   gotham = gotham_new(GOTHAM_CITIZEN_TYPE_BOTMAN,
                       SYSCONF_DIR"/botman.conf");
   if (!gotham)
     {
        CRI("Failed to create gotham object !");
        return EXIT_FAILURE;
     }

   gotham_reconnect_set(gotham, EINA_TRUE);

   ecore_event_handler_add(GOTHAM_EVENT_CONNECT, botman_event_connect, NULL);
   ecore_event_handler_add(GOTHAM_EVENT_DISCONNECT, botman_event_disconnect, NULL);
   ecore_event_handler_add(GOTHAM_EVENT_CITIZEN_LIST,
                           botman_event_citizen_list,
                           NULL);
   ecore_event_handler_add(GOTHAM_EVENT_CITIZEN_CONNECT,
                           botman_event_citizen_connect,
                           NULL);
   ecore_event_handler_add(GOTHAM_EVENT_CITIZEN_COMMAND,
                           botman_event_citizen_command,
                           NULL);

   ecore_main_loop_begin();

   gotham_free(gotham);
   gotham_shutdown();
   return EXIT_SUCCESS;
}

#undef ERR
#undef NFO
#undef DBG
#undef CRI
