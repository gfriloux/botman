#include <Eina.h>
#include <Gotham.h>
#include <Azy.h>
#include "Network_Common.h"

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#endif

int _module_network_log_dom;

typedef struct _Module_Network
{
   Gotham *gotham;

#ifdef _WIN32
   WSADATA wsadata;
#endif
} Module_Network;

#define CRI(...) EINA_LOG_DOM_CRIT(_module_network_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_module_network_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(_module_network_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_module_network_log_dom, __VA_ARGS__)

#define _MSG(_a, _b, _c)                                                       \
   do {                                                                        \
      if (_a->citizen->type == GOTHAM_CITIZEN_TYPE_ALFRED)                     \
        gotham_command_json_answer(".network", _b, EINA_TRUE, _c,              \
                           _a->citizen->gotham, _a->citizen, EINA_FALSE);      \
      else gotham_command_send(_a, _c);                                        \
   } while(0)
/* "debug" */

#ifdef _WIN32
void
network_get_win32(Module_Network *network, Gotham_Citizen_Command *command);
#endif
