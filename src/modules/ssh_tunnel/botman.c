#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <sys/file.h>

#include "ssh_tunnel.h"

typedef struct _Tunnel
{
   Module_Ssh_Tunnel *tunnel;
   Gotham_Citizen_Command *cmd;

   unsigned int error;
} Tunnel;

Tunnel *
_botman_ssh_tunnel_new(Module_Ssh_Tunnel *obj,
                       Gotham_Citizen_Command *cmd)
{
   Tunnel *tunnel;
   tunnel = calloc(1, sizeof(Tunnel));
   EINA_SAFETY_ON_NULL_RETURN_VAL(tunnel, NULL);
   tunnel->tunnel = obj;
   tunnel->cmd = cmd;
   return tunnel;
}

void
_botman_ssh_tunnel_free(Tunnel *tunnel)
{
   gotham_citizen_free(tunnel->cmd->citizen);
   gotham_command_free(tunnel->cmd);
   free(tunnel);
}

/**
 * @addtogroup Gotham_Module_Ssh_Tunnel
 * @{
 */

/**
 * @brief Check if stored PID for tunnel still exists.
 * If tunnel isn't valid (pid doesn't exist), reset variables.
 * @param obj Module object
 */
void
ssh_tunnel_check(Module_Ssh_Tunnel *obj)
{
#ifndef _WIN32
   Gotham_Citizen *citizen;
   struct stat st_tunnel;
   const char *file = NULL;
   Eina_Bool ret = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   citizen = obj->gotham->me;

   DBG("pid=%d, port=%d", obj->tunnel.pid, obj->tunnel.port);

   if (!obj->tunnel.pid)
     goto func_end;

   file = eina_stringshare_printf("/proc/%d/status", obj->tunnel.pid);
   if (stat(file, &st_tunnel))
     {
        // If proc does not exists, lock is invalid
        DBG("%s does not exists, tunnel does not exist anymore", file);
        goto func_end;
     }

   ret = EINA_TRUE;

func_end:
   if (file) eina_stringshare_del(file);

   /**
    * If PID is invalid, reset custom vars info
    */
   if (!ret)
     {
        obj->tunnel.pid = obj->tunnel.port = 0;
        VARSET("tunnel_pid", "%d", 0);
        VARSET("tunnel_port", "%d", 0);
     }
#endif
}

/**
 * @brief Informs sender that his tunnel has died.
 * @param obj Module object
 * @param cmd Gotham_Citizen_Command incoming command
 */
void
_tunnel_closed_send(Module_Ssh_Tunnel *obj,
                    Gotham_Citizen_Command *cmd)
{
   Eina_Strbuf *buf;
   Gotham_Citizen *citizen = obj->gotham->me;

   if (!obj->tunnel.pid)
     {
        NFO("Citizen has already been informed, doing nothing");
        return;
     }
   buf = eina_strbuf_new();

   eina_strbuf_append(buf, "Tunnel has died");
   gotham_command_json_answer(".ssh", cmd->command[1], EINA_TRUE, buf,
                              obj->gotham, cmd->citizen, EINA_TRUE);

   /** Reset variables and custom vars */
   obj->tunnel.pid = obj->tunnel.port = 0;
   VARSET("tunnel_pid", "%d", 0);
   VARSET("tunnel_port", "%d", 0);

   eina_strbuf_free(buf);
   return;
}

#ifdef _WIN32
Eina_Bool
_botman_ssh_tunnel_wait(void *data)
{
   Tunnel *tunnel = data;
   HANDLE h;
   Eina_Bool r;
   LARGE_INTEGER s;
   char buf[1024] = {0};
   DWORD l;

   /* Read MODULE_SSH_LOG */
   h = CreateFile(MODULE_SSH_LOG, GENERIC_READ, FILE_SHARE_READ, NULL,
                  OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
   EINA_SAFETY_ON_TRUE_GOTO(h == INVALID_HANDLE_VALUE, error);

   r = (Eina_Bool)GetFileSizeEx(h, &s);
   EINA_SAFETY_ON_TRUE_GOTO(!r, close_handle);
   EINA_SAFETY_ON_TRUE_GOTO(!s.QuadPart, close_handle);

   r = (Eina_Bool)ReadFile(h, buf, sizeof(buf)-1, &l, NULL);
   EINA_SAFETY_ON_TRUE_GOTO(!r, close_handle);

   _botman_ssh_tunnel_analyze(tunnel, buf);

   return EINA_FALSE;

close_handle:
   CloseHandle(h);
error:
   if (tunnel->error++ >= 10)
     {
        ERR("Failed to access log file");
        return EINA_FALSE;
     }
   return EINA_TRUE;
}
#endif

void
_botman_ssh_tunnel_analyze(Tunnel *tunnel,
                           const char *s)
{
   Eina_Strbuf *buf;
   unsigned int nb_params;
   char **params;
   Eina_Bool ok = EINA_FALSE;
   Gotham_Citizen *citizen = obj->gotham->me;

   buf = eina_strbuf_new();

   if (strncmp(s, "Allocated port",  strlen("Allocated port")))
     {
default_buf:
        eina_strbuf_append(buf, s);
        eina_strbuf_replace_all(buf, "\r", "");
        eina_strbuf_replace_all(buf, "\n", "");
        goto send_buf;
     }

   params = eina_str_split_full(s, " ", 50, &nb_params);

   if (nb_params < 3) goto default_buf;

   tunnel->tunnel->tunnel.port = atoi(params[2]);
   free(params[0]);
   free(params);

   eina_strbuf_append_printf(buf, "Tunnel opened on port %d, pid %d",
                             tunnel->tunnel->tunnel.port, tunnel->tunnel->tunnel.pid);
   VARSET("tunnel_pid", "%d", tunnel->tunnel->tunnel.pid);
   VARSET("tunnel_port", "%d", tunnel->tunnel->tunnel.port);
   ok = EINA_TRUE;
   obj->save_conf();

send_buf:
   gotham_command_json_answer(".ssh", "on", ok, buf, obj->gotham, cmd->citizen, EINA_TRUE);
   eina_strbuf_free(buf);
}

/**
 * @brief Callback for ecore_exe_pipe_run when receiving data.
 * This function gets called by ecore_main_loop everytime
 * the running tunnel is printing something to stdout.
 * We will send back to tunnel openner (we got his JID)
 * every data.
 * When we get tunnel port from stdout, we store it to custom vars.
 *
 * @param data Module object
 * @param type Event type UNUSED
 * @param event Ecore_Exe structure
 *
 * @return EINA_TRUE. Always.
 */
Eina_Bool
ssh_tunnel_cb_data(void *data,
                   int type EINA_UNUSED,
                   void *event)
{
   Module_Ssh_Tunnel *obj = data;
   Eina_Strbuf *buf;
   Ecore_Exe_Event_Data *datafromprocess = (Ecore_Exe_Event_Data *)event;
   Tunnel *tunnel = ecore_exe_data_get(datafromprocess->exe);

   if (obj != tunnel->tunnel) return ECORE_CALLBACK_PASS_ON;

   DBG("obj=%p", obj);

   buf = eina_strbuf_new();
   eina_strbuf_append_n(buf, datafromprocess->data, datafromprocess->size);
   _botman_ssh_tunnel_analyze(tunnel->tunnel, tunnel->cmd, eina_strbuf_string_get(buf));
   eina_strbuf_free(buf);
   return EINA_TRUE;
}

/**
 * @brief Callback for ecore_exe_pipe_run, when ending process.
 * This function is called when the tunnel stops running (due to an
 * error or not).
 * We will notify to user that tunnel has died, then we'll be able to
 * free the corresponding objects and reset object variables and
 * custom vars.
 * @param data Module object
 * @param type Event type UNUSED
 * @param event Ecore_Exe structure
 * @return EINA_TRUE. Always.
 */
Eina_Bool
ssh_tunnel_cb_end(void *data,
                  int type EINA_UNUSED,
                  void *event)
{
   Module_Ssh_Tunnel *obj = data;
   Ecore_Exe_Event_Del *datafromprocess = (Ecore_Exe_Event_Del *)event;
   Tunnel *tunnel;

   DBG("datafromprocess[%p] datafromprocess->exe[%p]", datafromprocess, datafromprocess->exe);
   if (!datafromprocess->exe) return EINA_TRUE;

   tunnel = ecore_exe_data_get(datafromprocess->exe);
   if (obj != tunnel->tunnel) return ECORE_CALLBACK_PASS_ON;
   _tunnel_closed_send(tunnel->tunnel, tunnel->cmd);
   _botman_ssh_tunnel_free(tunnel);
   return EINA_TRUE;
}


/**
 * @brief Open a ssh tunnel from bunker to SDS and return the allocated port.
 * If a tunnel is already open, don't do anything but return the port.
 * Otherwise, duplicate incoming object (containing command and citizen)
 * and give it as event_data to ecore_exe_pipe_run.
 * Also store the process id in Module's object.
 * @param data Module_Ssh_Tunnel object
 * @param command Gotham_Citizen_Command incoming command
 */
void
ssh_tunnel_on(void *data,
              Gotham_Citizen_Command *command)
{
   Module_Ssh_Tunnel *obj = data;
   Eina_Strbuf *buf;
   Gotham_Citizen *ctz;
   Gotham_Citizen_Command *cmd;
   Tunnel *tunnel;

   DBG("obj[%p] command[%p]", obj, command);

   buf = eina_strbuf_new();

   if (obj->tunnel.pid && obj->tunnel.port)
     {
        eina_strbuf_append_printf(buf, "Tunnel opened on port %d, pid %d ",
                                       obj->tunnel.port,
                                       obj->tunnel.pid);

        gotham_command_json_answer(".ssh", "on", EINA_TRUE, buf,
                                   obj->gotham, command->citizen, EINA_TRUE);

        eina_strbuf_free(buf);
        return;
     }

   /* Duplicate citizen and command */
   ctz = gotham_citizen_new(obj->gotham, command->citizen->jid);
   cmd = gotham_command_new(ctz, ".ssh on", command->jid);

#ifdef _WIN32
   /* We start a new ssh tunnel */
   eina_strbuf_append_printf(buf,
                             "ssh -p %u -i \"%s\" -N -R0:localhost:3389 "
                             "-l \"%s\" \"%s\" >%s 2>&1",
                             obj->conf->port, obj->conf->key,
                             obj->conf->login, obj->conf->host,
                             obj->tunnel.logfile);
#else
   eina_strbuf_append_printf(buf,
                             "ssh -p %u -i \"%s\" -N -R0:localhost:22 "
                             "-l \"%s\" \"%s\" 2>&1",
                             obj->conf->port, obj->conf->key,
                             obj->conf->login, obj->conf->host);
#endif
   DBG("Issueing command : %s", eina_strbuf_string_get(buf));

   tunnel = _botman_ssh_tunnel_new(obj, cmd);

   obj->tunnel.exe = ecore_exe_pipe_run(eina_strbuf_string_get(buf),
                                        ECORE_EXE_PIPE_READ_LINE_BUFFERED |
                                        ECORE_EXE_PIPE_READ               |
                                        ECORE_EXE_PIPE_ERROR_LINE_BUFFERED,
                                        cmd);
   DBG("obj->tunnel.exe[%p]", obj->tunnel.exe);
   eina_strbuf_free(buf);

   ecore_exe_data_set(obj->tunnel.exe, tunnel);

#ifdef _WIN32
   ecore_timer_add(0.5, _botman_ssh_tunnel_wait, tunnel);
#endif

   obj->tunnel.pid = ecore_exe_pid_get(obj->tunnel.exe);

   return;
}

/**
 * @brief This function will close the running tunnel.
 * In reality, it will close the child of the pid we know, as
 * the real tunnel will be a child of bash (and if we kill bash,
 * the tunnel will get PID#1 as PPID and will never ends).
 * @param data Module_Ssh_Tunnel object
 * @param command Gotham_Citizen_Command incoming command
 */
void
ssh_tunnel_off(void *data,
               Gotham_Citizen_Command *command)
{
   Module_Ssh_Tunnel *obj = data;
   Eina_Strbuf *buf;
   const char *cmd;

   DBG("obj[%p]", obj);

   buf = eina_strbuf_new();

   if (!obj->tunnel.pid || !obj->tunnel.port)
     {
        eina_strbuf_append(buf, "No tunnel found");
        gotham_command_json_answer(".ssh", "off", EINA_FALSE, buf,
                                   obj->gotham, command->citizen, EINA_TRUE);
        eina_strbuf_free(buf);
        return;
     }

   cmd = eina_stringshare_printf("kill -9 $(pgrep -P %d) %d",
                                 obj->tunnel.pid, obj->tunnel.pid);
   DBG("cmd[%p][%s]", cmd, cmd);

   system((char *)cmd);
   eina_stringshare_del(cmd);

   _tunnel_closed_send(obj, command);
}

/**
 * @brief Returns the current tunnel's port if opened.
 * @param data Module_Ssh_Tunnel object
 * @param command Gotham_Citizen_Command incoming command
 */
void
ssh_tunnel_get(void *data,
               Gotham_Citizen_Command *command)
{
   Module_Ssh_Tunnel *obj = data;
   Eina_Strbuf *buf;

   if (command->command[1]) return;

   buf = eina_strbuf_new();

   if (!obj->tunnel.pid || !obj->tunnel.port)
     eina_strbuf_append(buf, "No tunnel found");
   else
     eina_strbuf_append_printf(buf, "Tunnel opened on port %d, pid %d",
                                    obj->tunnel.port, obj->tunnel.pid);

   gotham_command_json_answer(".ssh", "", EINA_TRUE, buf,
                              obj->gotham, command->citizen, EINA_TRUE);

   eina_strbuf_free(buf);
}

/**
 * @}
 */
