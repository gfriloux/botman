#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <time.h>

#include "install.h"

/**
 * @addtogroup Gotham_Module_Install
 * @{
 */

/**
 * @brief Adds a thread (job) to the hash list.
 * @param install Module_Install object
 * @param exe Ecore_Exe identifier
 * @param jid Asker's Jabber ID
 * @param args Requested command & parameters
 * @return EINA_TRUE
 */
Eina_Bool
job_add(Module_Install *install,
        Ecore_Exe *exe,
        const char *jid,
        const char **args)
{
   const char *id = NULL;
   Module_Install_Job *job;
   unsigned int i;
   Eina_Strbuf *buf;

   DBG("Adding new thread to the list : [%p]", exe);

   job = calloc(1, sizeof(Module_Install_Job));
   job->exe = exe;
   job->jid = strdup(jid);

   buf = eina_strbuf_new();
   for (i=0; args[i]; i++)
     eina_strbuf_append_printf(buf, "%s ", args[i]);
   eina_strbuf_trim(buf);

   job->cmd = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);

   job->ts = time(0);
   id = eina_stringshare_printf("%ld", (long)ecore_exe_pid_get(exe));
   eina_hash_add(install->jobs, id, job);
   return EINA_TRUE;
}

/**
 * @brief Removes a thread (job) from the list.
 * This function is called when the thread ends, or when a user wants to
 * kill some running process
 * @param install Module_Install object
 * @param exe Ecore_Exe to remove
 * @return EINA_TRUE
 */
Eina_Bool
job_del(Module_Install *install,
        Ecore_Exe *exe)
{
   DBG("Removing thread from list : [%p]", exe);
   const char *id = NULL;
   id = eina_stringshare_printf("%ld", (long)ecore_exe_pid_get(exe));
   eina_hash_del(install->jobs, id, NULL);
   eina_stringshare_del(id);
   return EINA_TRUE;
}


/**
 * @brief Find a job (this is not related with pole-emploi.fr).
 * Search in hash list if the current thread still exists
 * @param install Module_Install object
 * @param exe Ecore_Exe to search for
 * @return EINA_TRUE if still exists, otherwise EINA_FALSE
 */
Eina_Bool
job_find(Module_Install *install,
         Ecore_Exe *exe)
{
   Module_Install_Job *job;
   const char *id = NULL;
   DBG("Searching thread in list : [%p]", exe);
   id = eina_stringshare_printf("%ld", (long)ecore_exe_pid_get(exe));
   job = eina_hash_find(install->jobs, id);
   eina_stringshare_del(id);

   return (job ? EINA_TRUE : EINA_FALSE);
}

/**
 * @brief Removes a thread (job) from the list.
 * This function is called when the thread ends, or when a user wants to
 * kill some running process
 * @param install Module_Install object
 * @param ex Ecore_Exe to remove
 * @return EINA_TRUE on success, otherwise EINA_FALSE
 */
Eina_Bool
job_kill(Module_Install *install,
         const char *id)
{
   Module_Install_Job *job;
   DBG("Killing thread (id %s)", id);
   job = eina_hash_find(install->jobs, id);
   if (!job)
     {
        ERR("Thread #%s not found !", id);
        return EINA_FALSE;
     }
   ecore_exe_kill(job->exe);
   job_del(install, job->exe);

   return EINA_TRUE;
}

/**
 * @brief Callback for printing running threads list.
 * @param hash Eina_Hash list
 * @param key hash's key
 * @param data struct _exe object to print
 * @param fdata Eina_List in which we write results
 * @return EINA_TRUE
 */
static Eina_Bool
_job_item_get(const Eina_Hash *hash,
              const void *key,
              void *data,
              void *fdata)
{
#define _FMT(n,t)                                                              \
{                                                                              \
   if (n)                                                                      \
     {                                                                         \
         eina_strbuf_append_printf(buf, "%d %s", n, t);                        \
         if (n > 1) eina_strbuf_append(buf, "s");                              \
         eina_strbuf_append(buf, " ");                                         \
     }                                                                         \
}
   Eina_List **l = fdata;
   Module_Install_Job *j = data;
   Eina_Strbuf *buf;
   time_t elapsed;
   struct tm *elapsed_tm;

   DBG("hash[%p], key[%p], data[%p], fdata[%p]", hash, key, data, fdata);

   buf = eina_strbuf_new();
   gmtime(&(j->ts));

   eina_strbuf_append(buf, "(");

   elapsed = time(0)-(j->ts);
   elapsed_tm = gmtime(&elapsed);

   DBG("Elapsed time : %d jour(s) %.2d heures %.2d minutes %.2d secondes",
       elapsed_tm->tm_yday,
       elapsed_tm->tm_hour,
       elapsed_tm->tm_min,
       elapsed_tm->tm_sec);

   // Formatting elapsed time
   _FMT(elapsed_tm->tm_yday, "jour");
   _FMT(elapsed_tm->tm_hour, "heure");
   _FMT(elapsed_tm->tm_min, "min");
   _FMT(elapsed_tm->tm_sec, "sec");
   eina_strbuf_rtrim(buf);
   eina_strbuf_append(buf, ") ");

   eina_strbuf_append_printf(buf, "ID:“%ld”, Cmd:“%s”, From:“%s”",
                                  (long)ecore_exe_pid_get(j->exe),
                                  j->cmd,
                                  j->jid);

   DBG("%s", eina_strbuf_string_get(buf));

   *l = eina_list_append(*l, eina_strbuf_string_steal(buf));
   eina_strbuf_free(buf);
   return EINA_TRUE;
#undef _FMT
}

/**
 * @brief List running threads (jobs) in hash list
 * @param install Module_Install object
 * @return Eina_List, contains a formatted line for each running thread
 */
Eina_List *
jobs_list(Module_Install *install)
{
   Eina_List *l = NULL;

   eina_hash_foreach(install->jobs, _job_item_get, &l);

   return l;
}


/**
 * @}
 */
