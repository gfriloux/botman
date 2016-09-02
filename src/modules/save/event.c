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
 * @brief Reply backup interval and last backup time.
 * @param save Module_Save object
 * @param command Gotham_Citizen_Command incoming command
 */
void
event_citizen_save_info(void *data,
                        Gotham_Citizen_Command *command)
{
   Module_Save *save = data;
   const char *love;
   char file[PATH_MAX],
        date[25];
   Eina_Strbuf *buf;
   Eina_File *ef;
   size_t file_size;
   time_t file_time;
   struct tm temp;
   Eina_Bool am_i_alfred = EINA_FALSE;

   if (command->command[1]) return;

   snprintf(file, sizeof(file), "%s%.64s.%.255s.save",
            MODULE_SAVE_BACKUP,
            save->gotham->conf->xmpp.login,
            save->gotham->conf->xmpp.server);
   ef = eina_file_open(file, EINA_FALSE);
   if (!ef)
     {
        gotham_command_send(command, "No backups found");
        return;
     }

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

   gotham_command_send(command, eina_strbuf_string_get(buf));
   eina_strbuf_free(buf);
}

/**
 * @brief Change the backup interval.
 * Once updated, save new conf to file.
 * @param save Module_Save object
 * @param command const char ** incoming command
 */
void
event_citizen_save_set(void *data,
                       Gotham_Citizen_Command *command)
{
   Module_Save *save = data;

   if ((!command->command[2])                    ||
       (!command->command[3])                    ||
       (strcmp(command->command[2], "interval")) ||
       (!utils_isnumber(command->command[3][0])))
     {
        gotham_command_send(command, "Wrong command");
        return;
     }

   save->interval = atoi(command->command[3]);
   conf_save(save);

   gotham_command_send(command, "Modification done.");
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

   conf_restore(save);
   return EINA_TRUE;
}

#undef AUTH

/**
 * @}
 */
