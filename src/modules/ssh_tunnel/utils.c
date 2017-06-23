#include "ssh_tunnel.h"

char *
ssh_tunnel_utils_ssh_command(Module_Ssh_Tunnel *obj)
{
   char *s;

#ifdef _WIN32
   {
      char *key;
      char cmd[2048] = {0};
      char *exe = NULL;

      key = escarg(ESCARG_MODE_WINDOWS, "%s", obj->conf->key);

      if (!obj->conf->command[0]) exe = "ssh";
      else exe = obj->conf->command;

      snprintf(cmd, sizeof(cmd)-1,
               "%s -p %u -i %s -N -R0:localhost:%d -l %s %s",
               exe, obj->conf->port, key, obj->conf->forwarded_port,
               obj->conf->login, obj->conf->host);
      free(key);
      s = strdup(cmd);
   }
#else
   s = escarg(ESCARG_MODE_POSIX,
              "ssh -p %u -i %s -N -R0:localhost:%d -l %s %s",
              obj->conf->port, obj->conf->key, obj->conf->forwarded_port,
              obj->conf->login, obj->conf->host);
#endif

   return s;
}
