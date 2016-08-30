#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <cJSON.h>
#include <sys/file.h>

#include "ssh_tunnel.h"

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
   Eina_Strbuf *buf,
               *data_buf;
   Eina_Bool ok = EINA_FALSE;

   Ecore_Exe_Event_Data *datafromprocess = (Ecore_Exe_Event_Data *)event;
   Gotham_Citizen_Command *cmd = ecore_exe_data_get(datafromprocess->exe);
   Gotham_Citizen *citizen = obj->gotham->me;

   if (obj->tunnel.exe != datafromprocess->exe)
     return ECORE_CALLBACK_PASS_ON;

   DBG("obj=%p", obj);

   buf = eina_strbuf_new();
   data_buf = eina_strbuf_new();

   eina_strbuf_append_n(data_buf,
                        datafromprocess->data,
                        datafromprocess->size);
   eina_strbuf_append(data_buf, "");

   if (!strncmp(eina_strbuf_string_get(data_buf),
                "Allocated port",
                strlen("Allocated port")))
     {
        unsigned int nb_params = 0;
        char **params = eina_str_split_full(eina_strbuf_string_get(data_buf),
                                            " ", 50, &nb_params);

        if (nb_params < 3)
           {
              eina_strbuf_free(data_buf);
              free(params[0]);
              free(params);
              return EINA_TRUE;
           }

        obj->tunnel.port = atoi(params[2]);
        free(params[0]);
        free(params);

        eina_strbuf_append_printf(buf, "Tunnel opened on port %d, pid %d",
                                       obj->tunnel.port, obj->tunnel.pid);
        VARSET("tunnel_pid", "%d", obj->tunnel.pid);
        VARSET("tunnel_port", "%d", obj->tunnel.port);

        obj->save_conf();

        ok = EINA_TRUE;
     }
   else
     {
        eina_strbuf_replace_all(data_buf, "\r", "");
        eina_strbuf_replace_all(data_buf, "\n", "");
        eina_strbuf_append(buf, eina_strbuf_string_get(data_buf));
     }

   eina_strbuf_free(data_buf);
   gotham_command_json_answer(".ssh", cmd->command[1], ok, buf,
                              obj->gotham, cmd->citizen, EINA_TRUE);
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
   Gotham_Citizen_Command *cmd;

   DBG("datafromprocess[%p] datafromprocess->exe[%p]",
       datafromprocess, datafromprocess->exe);

   if (!datafromprocess->exe) return EINA_TRUE;

   cmd = ecore_exe_data_get(datafromprocess->exe);

   if (obj->tunnel.exe != datafromprocess->exe)
     return ECORE_CALLBACK_PASS_ON;

   _tunnel_closed_send(obj, cmd);

   gotham_citizen_free(cmd->citizen);
   gotham_command_free(cmd);

   return EINA_TRUE;
}


/**
 * @brief Open a ssh tunnel from bunker to SDS and return the allocated port.
 * If a tunnel is already open, don't do anything but return the port.
 * Otherwise, duplicate incoming object (containing command and citizen)
 * and give it as event_data to ecore_exe_pipe_run.
 * Also store the process id in Module's object.
 * @param obj Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
ssh_tunnel_open(Module_Ssh_Tunnel *obj,
                Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;
   Gotham_Citizen *ctz;
   Gotham_Citizen_Command *cmd;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   EINA_SAFETY_ON_NULL_RETURN(command);

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

   /* We start a new ssh tunnel */
   eina_strbuf_append_printf(buf,
                             "ssh -p %u -i \"%s\" "
                             "-N -R0:localhost:22 "
                             "-l \"%s\" \"%s\" 2>&1",
                             obj->infos.port, obj->infos.key,
                             obj->infos.login, obj->infos.host);
   DBG("Issueing command : %s", eina_strbuf_string_get(buf));
   obj->tunnel.exe = ecore_exe_pipe_run(eina_strbuf_string_get(buf),
                                        ECORE_EXE_PIPE_READ_LINE_BUFFERED |
                                        ECORE_EXE_PIPE_READ,
                                        cmd);
   eina_strbuf_free(buf);

   obj->tunnel.pid = ecore_exe_pid_get(obj->tunnel.exe);

   return;
}

/**
 * @brief This function will close the running tunnel.
 * In reality, it will close the child of the pid we know, as
 * the real tunnel will be a child of bash (and if we kill bash,
 * the tunnel will get PID#1 as PPID and will never ends).
 * @param obj Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
ssh_tunnel_close(Module_Ssh_Tunnel *obj,
                 Gotham_Citizen_Command *command)
{
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
 * @param obj Module object
 * @param command Gotham_Citizen_Command incoming command
 */
void
ssh_tunnel_get(Module_Ssh_Tunnel *obj,
               Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;

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
