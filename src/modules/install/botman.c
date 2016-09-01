#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <cJSON.h>
#include <ctype.h>

#include "install.h"

/**
 * @addtogroup Gotham_Module_Install
 * @{
 */

/**
 * @brief Send answer in JSON format.
 * @param cmd Command requested
 * @param params Command parameters
 * @param status EINA_TRUE on success, EINA_FALSE otherwise
 * @param gotham Gotham structure
 * @param content String buffer containing message content
 * @param citizen Gotham_Citizen we reply to
 * @param send_to_alfred EINA_TRUE if message has to be also sent
          to Alfred, EINA_FALSE otherwise
 */
void
module_install_json_answer(const char *cmd,
                           const char *params,
                           Eina_Bool status,
                           Eina_Strbuf *content,
                           Gotham *gotham,
                           Gotham_Citizen *citizen,
                           Eina_Bool send_to_alfred)
{
   cJSON *json_obj,
         *json_content;
   char **split;
   unsigned int i;
   char *ptr;

   json_obj = cJSON_CreateObject();
   cJSON_AddStringToObject(json_obj, "command", cmd);
   cJSON_AddStringToObject(json_obj, "parameters", params);
   cJSON_AddStringToObject(json_obj, "status", status ? "ok" : "ko");

   json_content = cJSON_CreateArray();
   split = eina_str_split(eina_strbuf_string_get(content), "\n", 0);
   for (i=0; split[i]; i++)
     if (split[i][0])
       cJSON_AddItemToArray(json_content, cJSON_CreateString(split[i]));

   cJSON_AddItemToObject(json_obj, "content", json_content);
   if (split[0]) free(split[0]);
   free(split);

   ptr = cJSON_Print(json_obj);
   cJSON_Delete(json_obj);
   gotham_citizen_send(citizen, ptr);

   if (send_to_alfred && (strcmp(citizen->jid, gotham->alfred->jid)))
     gotham_citizen_send(gotham->alfred, ptr);

   free(ptr);
   return;
}

Eina_Bool
_this_is_my_process(Module_Install *install, Ecore_Exe *process_exe)
{
   Eina_Bool found = EINA_FALSE;
   Eina_Iterator *it;
   void *data;

   it = eina_hash_iterator_data_new(install->jobs);
   while (eina_iterator_next(it, &data))
     {
       Module_Install_Job *job = data;
       if (job->exe != process_exe)
         continue;
       found = EINA_TRUE;
       break;
     }
   eina_iterator_free(it);
   return found;
}

/**
 * @brief Data callback for ecore_exe_pipe_run.
 * Appends data to the Eina_Strbuf obj->buf, and sets the status to EINA_FALSE
 * if the event type is ECORE_EXE_EVENT_ERROR
 * @param data UNUSED
 * @param type UNUSED
 * @param event Ecore_Exe_Event_Data containing our context object
 *              (a Module_Install_Cmd object)
 * @return ECORE_CALLBACK_RENEW
 */
static Eina_Bool
_install_data_cb(void *data,
                 int type EINA_UNUSED,
                 void *event)
{
   Module_Install *install = data;
   Ecore_Exe_Event_Data *datafromprocess = (Ecore_Exe_Event_Data *)event;
   Module_Install_Cmd *obj = ecore_exe_data_get(datafromprocess->exe);

   if (!_this_is_my_process(install, datafromprocess->exe))
     return ECORE_CALLBACK_PASS_ON;

   DBG("obj[%p]", obj);

   if (datafromprocess->data)
     eina_strbuf_append_length(obj->buf,
                               datafromprocess->data,
                               datafromprocess->size);

   if (type == ECORE_EXE_EVENT_ERROR)
     obj->ok = EINA_FALSE;

   return ECORE_CALLBACK_RENEW;
}


/**
 * @brief End callback for ecore_exe_pipe_run command.
 * Checks how process has terminated and then send back command result to
 * sender.
 * @param data UNUSED
 * @param type UNUSED
 * @param event Ecore_Exe_Event_Del containing our context object
 *              (a Module_Install_Cmd object)
 * @return ECORE_CALLBACK_RENEW
 */
static Eina_Bool
_install_del_cb(void *data,
                int type EINA_UNUSED,
                void *event)
{
   Module_Install *install = data;
   Ecore_Exe_Event_Del *datafromprocess = (Ecore_Exe_Event_Del *)event;
   Module_Install_Cmd *obj = ecore_exe_data_get(datafromprocess->exe);

   Eina_Strbuf *buf;
   unsigned int i;

   if (!_this_is_my_process(install, datafromprocess->exe))
     return ECORE_CALLBACK_PASS_ON;

   DBG("obj[%p]", obj);

   if (datafromprocess->signalled)
     {
        eina_strbuf_append(obj->buf, "\nProcess terminated by signal");
        obj->ok = EINA_FALSE;
     }
   else if (datafromprocess->exit_code)
     obj->ok = EINA_FALSE;


   buf = eina_strbuf_new();
   for (i=1; obj->command[i]; i++)
     eina_strbuf_append_printf(buf, "%s ", obj->command[i]);
   eina_strbuf_trim(buf);

   module_install_json_answer(obj->command[0],
                              eina_strbuf_string_get(buf),
                              obj->ok,
                              obj->buf,
                              obj->install->gotham,
                              obj->citizen,
                              EINA_TRUE);

   eina_strbuf_free(buf);

   job_del(obj->install, datafromprocess->exe);

   module_install_cmd_free(obj);

   return ECORE_CALLBACK_RENEW;
}

/**
 * @brief List running jobs.
 * Each install / upgrade task is registered as a job when it starts,
 * and removed when it ends (ok or error).
 * List jobs currently running and reply jobs list to citizen who asked.
 * @param install Module_Install object
 * @param command Gotham_Citizen_Command
 */
void
module_install_jobs_list(Module_Install *install,
                         Gotham_Citizen_Command *command)
{
   Eina_List *jobs = NULL;
   char *job_line = NULL;
   Eina_Strbuf *buf;

   DBG(".jobs requested from “%s”", command->citizen->jid);

   jobs = jobs_list(install);

   buf = eina_strbuf_new();

   if (!eina_list_count(jobs))
     {
        eina_strbuf_append(buf, "No threads running");
        goto answer;
     }

   EINA_LIST_FREE(jobs, job_line)
     {
        eina_strbuf_append(buf, job_line);
        free(job_line);
     }

answer:
   module_install_json_answer(command->command[0],
                              "",
                              EINA_TRUE,
                              buf,
                              install->gotham,
                              command->citizen,
                              EINA_FALSE);
   eina_strbuf_free(buf);

   return;
}

/**
 * @brief Kill a running job.
 * And return killing status to whoever asked.
 * @param install Module_Install object
 * @param command Gotham_Citizen_Command object
 */
void
module_install_job_kill(Module_Install *install,
                        Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;
   Eina_Bool success = EINA_FALSE;

   buf = eina_strbuf_new();
   if (!command->command[1])
     {
        eina_strbuf_append(buf, "You must provide a thread_id to kill.\n"
                                "Type .jobs to see running threads.");
        goto answer;
     }

   if (!job_kill(install, command->command[1]))
     {
        eina_strbuf_append_printf(buf, "Unable to kill thread #%s.",
                                       command->command[1]);
        goto answer;
     }

   success = EINA_TRUE;
   eina_strbuf_append_printf(buf, "Thread #%s killed.", command->command[1]);

answer:
   module_install_json_answer(command->command[0],
                              command->command[1],
                              success,
                              buf,
                              install->gotham,
                              command->citizen,
                              EINA_FALSE);
   eina_strbuf_free(buf);

   return;
}

/**
 * @brief Install commands governor (install / update / list / kill).
 * For a jobs list, call module_install_jobs_list.
 * For a kill, call module_install_job_kill.
 * For install / upgrade : build command to exec (pre_cmd, cmd and post_cmd
 * are defined in install module's conf file), then run command in a pipe
 * (ecore_exe_pipe_run) and register this job in jobs list.
 * @param install Module_Install object
 * @param type Action type (see @ref Module_Install_Type)
 * @param command Gotham_Citizen_Command object
 */
void
module_install_event_botman_command(Module_Install *install,
                                    Module_Install_Type type,
                                    Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;
   Eina_Hash *h = NULL;
   Module_Install_Cmd *obj;
   const char **install_cmd = command->command;
   const char *pre,
              *cmd,
              *post,
              *args = NULL;
   unsigned int i;
   Ecore_Exe *exe;

   EINA_SAFETY_ON_NULL_RETURN(install);
   EINA_SAFETY_ON_NULL_RETURN(command);

   DBG("install[%p], command[%p]", install, command);

   if (type == MODULE_INSTALL_TYPE_JOBS)
     {
        module_install_jobs_list(install, command);
        return;
     }
   if (type == MODULE_INSTALL_TYPE_KILL)
     {
        module_install_job_kill(install, command);
        return;
     }

   /**
    * Get hash data
    */
   if (type == MODULE_INSTALL_TYPE_INSTALL)
     h = install->install;
   else if (type == MODULE_INSTALL_TYPE_UPGRADE)
     h = install->upgrade;

   DBG("Searching keys in h[%p] (%d vars)", h, eina_hash_population(h));

   pre = eina_hash_find(h, "pre");
   cmd = eina_hash_find(h, "cmd");
   post = eina_hash_find(h, "post");

   if (!cmd)
     {
        ERR("Wutzufuk ?! No command ?!");
        return;
     }

   /**
    * Reply to sender with the install_begins message, informing that install
    * will begin in a few seconds.
    * Do not reply if Alfred was the sender
    */
   if (command->citizen->type != GOTHAM_CITIZEN_TYPE_ALFRED)
     {
        Eina_Strbuf *bufargs = eina_strbuf_new();
        for (i=1; install_cmd[i]; i++)
          eina_strbuf_append_printf(bufargs, "%s ", install_cmd[i]);
        eina_strbuf_trim(bufargs);

        buf = eina_strbuf_new();
        eina_strbuf_append(buf, "Software(s) install/upgrade will begin "
                                "in a few seconds.");

        module_install_json_answer(".install_begins",
                                   eina_strbuf_string_get(bufargs),
                                   EINA_TRUE,
                                   buf,
                                   install->gotham,
                                   command->citizen,
                                   EINA_FALSE);

        eina_strbuf_free(buf);
        eina_strbuf_free(bufargs);
     }

   /**
    * Gather args
    */
   if (type == MODULE_INSTALL_TYPE_INSTALL)
     {
        buf = eina_strbuf_new();
        for (i=1; install_cmd[i]; i++)
          eina_strbuf_append_printf(buf, "%s ", install_cmd[i]);
        eina_strbuf_trim(buf);
        args = eina_strbuf_string_steal(buf);
        eina_strbuf_free(buf);
     }

   /**
    * Build command line
    */
   buf = eina_strbuf_new();
   if (pre && pre[0])
     eina_strbuf_append_printf(buf, "%s && ", pre);
   eina_strbuf_append_printf(buf, cmd, args);
   if (post && post[0])
     eina_strbuf_append_printf(buf, " && %s", post);
   DBG("Command to send : \n%s", eina_strbuf_string_get(buf));

   /**
    * Duplicate objects into new structure Module_Install_Cmd
    * Define ecore_handlers for pipe
    */
   obj = module_install_cmd_new(install, command);

   exe = ecore_exe_pipe_run(eina_strbuf_string_get(buf),
                            ECORE_EXE_PIPE_WRITE |
                            ECORE_EXE_PIPE_READ |
                            ECORE_EXE_PIPE_ERROR,
                            obj);
   job_add(install, exe, command->citizen->jid, command->command);

   eina_strbuf_free(buf);

   return;
}



/**
 * @brief Creates a new Module_Install_Cmd object.
 * We use this function because install / upgrades are launched async;
 * at the moment they end, initial Gotham_Citizen_Command object may have
 * been freed. We have to duplicate citizen and command, and will free them
 * when process is terminated.
 * Also add ecore_event_handlers for data, error and del.
 * @param install Module_Install object
 * @param command Gotham_Citizen_Command object we want to duplicate
 *                (partially)
 * @return Newly created Module_Install_Cmd object
 */
Module_Install_Cmd *
module_install_cmd_new(Module_Install *install,
                       Gotham_Citizen_Command *command)
{
   unsigned int i,
                len = 0,
                count = 0;
   char *words,
        *p;
   Module_Install_Cmd *obj = calloc(1, sizeof(Module_Install_Cmd));
   obj->citizen = gotham_citizen_new(install->gotham,
                                     command->citizen->jid);
   obj->install = install;
   obj->buf = eina_strbuf_new();
   obj->ok = EINA_TRUE;

   /**
    * Copy command->command in new structure
    */
   for (i=0; command->command[i]; i++)
     {
        size_t params_len = strlen(command->command[i]);

        if (!params_len)
          continue;

        count++;

        if (len < params_len)
          len = params_len;
     }

   obj->command = calloc(count + 1, sizeof(char *));
   words = calloc(count, len + 1);

   for (i=0; command->command[i]; i++)
     {
        p = words + i * (len + 1);

        strcpy(p, command->command[i]);
        obj->command[i] = p;
     }

   obj->evd = ecore_event_handler_add(ECORE_EXE_EVENT_DATA,
                                      _install_data_cb,
                                      install);
   obj->evr = ecore_event_handler_add(ECORE_EXE_EVENT_ERROR,
                                      _install_data_cb,
                                      install);
   obj->eve = ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
                                      _install_del_cb,
                                      install);

   return obj;
}

/**
 * @brief Free a Module_Install_Cmd object.
 * Delete ecore_event_handlers that have been declared when this object
 * was created, then free the structure.
 * @param obj Module_Install_Cmd object to free
 */
void
module_install_cmd_free(Module_Install_Cmd *obj)
{
   EINA_SAFETY_ON_NULL_RETURN(obj);
   ecore_event_handler_del(obj->evd);
   ecore_event_handler_del(obj->eve);
   ecore_event_handler_del(obj->evr);

   if (obj->buf)
     eina_strbuf_free(obj->buf);

   if (obj->command)
     free(obj->command);

   if (obj->citizen)
     gotham_citizen_free(obj->citizen);

   free(obj);
}

/**
 * @brief Register Botman commands.
 * Command list :<br />
 * .install softwarelist : install softwarelist on each citizen
 *                         that match pattern<br />
 * .upgrade [repository|all] : upgrade selected repository on
 *                             each citizen that match pattern<br />
 * .jobs : list jobs running on each citizen matching pattern<br />
 * .kill job_id : kill a running job identified by job_id on each
 *                citizen that match pattern<br />
 */
void
module_install_botman_commands_register(void)
{
   gotham_modules_command_add("install", ".install",
                              "[.install softwarelist] - "
                              "Install softwarelist.", NULL);
   gotham_modules_command_add("install", ".upgrade",
                              "[.upgrade] - "
                              "Performs an upgrade on all repositories.", NULL);
   gotham_modules_command_add("install", ".jobs",
                              "[.jobs] - "
                              "List running install / upgrade jobs", NULL);
   gotham_modules_command_add("install", ".kill",
                              "[.jobs job_id] - "
                              "Kill running install / upgrade job.", NULL);
}

/**
 * @brief Unregister Botman's commands.
 * For each command to unregister call @ref gotham_modules_command_del
 */
void
module_install_botman_commands_unregister(void)
{
   gotham_modules_command_del("install", ".install");
   gotham_modules_command_del("install", ".upgrade");
   gotham_modules_command_del("install", ".jobs");
   gotham_modules_command_del("install", ".kill");
}

/**
 * @}
 */
