#include "ssh_tunnel.h"

char *
ssh_tunnel_utils_ssh_command(Module_Ssh_Tunnel *obj)
{
   char *s;

#ifdef _WIN32
   {
      char *key;
      char cmd[2048] = {0};

      key = escarg(ESCARG_MODE_WINDOWS, "%s", obj->conf->key);

      snprintf(cmd, sizeof(cmd)-1,
               "ssh -p %u -i %s -N -R0:localhost:3389 -l %s %s",
               obj->conf->port, key, obj->conf->login,
               obj->conf->host);
      free(key);
      s = strdup(cmd);
   }
#else
   s = escarg(ESCARG_MODE_POSIX,
              "ssh -p %u -i %s -N -R0:localhost:22 -l %s %s",
              obj->conf->port, obj->conf->key, obj->conf->login,
              obj->conf->host);
#endif

   return s;
}
