#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <time.h>
#include "save.h"

/**
 * @addtogroup Gotham_Module_Save
 * @{
 */

/**
 * Authorization macro
 */
#define AUTH(_a, _b, _c)                                                       \
   {                                                                           \
      if ((_a) && (!_a(gotham_modules_command_get(_b), _c->citizen)))          \
        {                                                                      \
           gotham_command_send(_c, "Access denied");                           \
           return EINA_TRUE;                                                   \
        }                                                                      \
   }

/**
 * @brief Reply backup interval and last backup time.
 * @param save Module_Save object
 * @param command Gotham_Citizen_Command incoming command
 * @return const char * string to reply
 */
const char *
_event_citizen_save_info(Module_Save *save,
                         Gotham_Citizen_Command *command)
{
   const char *p,
              *love;
   char file[PATH_MAX],
        date[25];
   Eina_Strbuf *buf;
   Eina_File *ef;
   size_t file_size;
   time_t file_time;
   struct tm temp;
   Eina_Bool am_i_alfred = EINA_FALSE;

   snprintf(file, sizeof(file), "%s%.64s.%.255s.save",
            MODULE_SAVE_BACKUP,
            save->gotham->conf->xmpp.login,
            save->gotham->conf->xmpp.server);
   ef = eina_file_open(file, EINA_FALSE);
   if (!ef)
     return strdup("No backups found");

   file_size = eina_file_size_get(ef);
   file_time = eina_file_mtime_get(ef);
   eina_file_close(ef);

   localtime_r(&file_time, &temp);
   sprintf(date, "%04d/%02d/%02d at %02d:%02d:%02d", (temp.tm_year)+1900,
           (temp.tm_mon)+1, temp.tm_mday, temp.tm_hour, temp.tm_min,
           temp.tm_sec);

   if ((command->citizen->nickname) && (command->citizen->nickname[0]!=0))
     love = command->citizen->nickname;
   else
     love = command->citizen->jid;

   am_i_alfred = (save->gotham->me->type == GOTHAM_CITIZEN_TYPE_ALFRED);
   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf,
                             "Dear %s, "
                             "backup is made every %d seconds. "
                             "Last backup was on %s, for a size of %zu bytes. "
                             "Sincerly yours, %s.",
                             love,
                             save->interval, date, file_size,
                             am_i_alfred ? "Alfred" : "Botman");

   p = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return p;
}

/**
 * @brief Change the backup interval.
 * Once updated, save new conf to file.
 * @param save Module_Save object
 * @param command const char ** incoming command
 * @return const char * string to reply
 */
const char *
_event_citizen_save_set(Module_Save *save, const char **command)
{
   if ((!command[2])                    ||
       (!command[3])                    ||
       (strcmp(command[2], "interval")) ||
       (!utils_isnumber(command[3][0])))
     return strdup("Wrong command");

   save->interval = atoi(command[3]);
   conf_save(save);

   return strdup("Modification done.");
}

/**
 * @brief Callback when all modules are loaded.
 * Set the function pointer for access_allwed using
 * @ref gotham_modules_function_get
 * @param data Module_Save object
 * @param type UNUSED
 * @param ev UNUSED
 * @return EINA_TRUE
 */
Eina_Bool
event_modules_ready(void *data,
                    int type EINA_UNUSED,
                    void *ev EINA_UNUSED)
{
   Module_Save *save = data;

   save->access_allowed = gotham_modules_function_get("access",
                                                      "access_allowed");
   conf_restore(save);
   return EINA_TRUE;
}

/**
 * @brief Callback when a citizen sends a command.
 * Check citizen auth level / compare to the command access level. Citizen
 * level has to be at least equal to command level in order to run it.
 * @param data Module_Save object
 * @param type UNUSED
 * @param ev Gotham_Citizen_Command structure
 * @return EINA_TRUE
 */
Eina_Bool
event_citizen_command(void *data,
                      int type EINA_UNUSED,
                      void *ev)
{
   Module_Save *save = data;
   Gotham_Citizen_Command *command = ev;
   const char **save_command = command->command,
              *answer = NULL;

   DBG("save[%p] command[%p]=%s", save, command, command->name);

   if (strncmp(command->name, ".save", 5))
     return EINA_TRUE;

   if (!save_command[1])
     {
        command->handled = EINA_TRUE;
        AUTH(save->access_allowed, ".save", command);
        answer = _event_citizen_save_info(save, command);
     }
   else if (!strcmp(save_command[1], "set"))
     {
        command->handled = EINA_TRUE;
        AUTH(save->access_allowed, ".save set", command);
        answer = _event_citizen_save_set(save, save_command);
     }

   gotham_command_send(command,
                       (answer) ? answer : "Failed to execute command");
   free((char *)answer);
   return EINA_TRUE;
}

#undef AUTH

/**
 * @}
 */
