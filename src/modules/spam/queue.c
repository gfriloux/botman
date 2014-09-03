#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "spam.h"

/**
 * @addtogroup Gotham_Module_Spam
 * @{
 */


typedef struct _Queue
{
   Gotham_Citizen *from,
                  *to;
   const char *command;
   time_t timestamp;
} Queue; /*!< Queue commands structure */


/**
 * @brief Create a new Queue object and initialize it with provided values.
 * @param from Gotham_Citizen sender
 * @param to Gotham_Citizen recipient
 * @param command Command to send
 * @return Newly created Queue object
 */
Queue *
_queue_new(Gotham_Citizen *from,
           Gotham_Citizen *to,
           const char *command)
{
   Queue *q;

   DBG("from[%p]=%s to[%p]=%s command[%p]=%s",
       from, from->jid, to, to->jid, command, command);
   q = calloc(1, sizeof(Queue));
   q->from = from;
   q->to = to;
   q->command = strdup(command);
   q->timestamp = time(NULL);
   return q;
}

/**
 * @brief Free a Queue object
 * @param q Queue object to free
 */
void
_queue_free(Queue *q)
{
   free((char *)q->command);
   free(q);
}

/**
 * @brief Callback for queue timer.
 * Run through queue, if commands are found that mean they have timed out.
 * Inform sender that his command has timed out.
 * @param data Module_Spam object
 * @return EINA_TRUE
 */
Eina_Bool
queue_timer(void *data)
{
   Module_Spam *spam = data;
   Queue *q;
   Eina_Strbuf *b;
   Eina_List *l,
             *ln;

   DBG("spam[%p]", spam);

   EINA_LIST_FOREACH_SAFE(spam->queue.l, l, ln, q)
     {
        time_t now = time(NULL);

        DBG("times %.0f - %.0f", (double)q->timestamp, (double)now);

        if ((now - q->timestamp) < 300)
          continue;

        b = eina_strbuf_new();
        eina_strbuf_append_printf(b, "Command %s timed out on %s",
                                  q->command, q->to->jid);
        gotham_citizen_send(q->from, eina_strbuf_string_get(b));
        eina_strbuf_free(b);
        DBG("Removing queue for %s", q->command);
        spam->queue.l = eina_list_remove(spam->queue.l, q);
        _queue_free(q);
     }
   return EINA_TRUE;
}

/**
 * @brief Add a command to queue.
 * @param spam Module_Spam object
 * @param from Gotham_Citizen sender
 * @param to Gotham_Citizen recipient
 * @param command Command to send
 */
void
queue_add(Module_Spam *spam,
          Gotham_Citizen *from,
          Gotham_Citizen *to,
          const char *command)
{
   Queue *q;
   DBG("spam[%p] from[%p]=%s to[%p]=%s command[%p]=%s",
       spam, from, from->jid, to, to->jid, command, command);

   q = _queue_new(from, to, command);
   spam->queue.l = eina_list_append(spam->queue.l, q);
}

/**
 * @brief Manage return from a .spam command.
 * Run through queue to find the matching command, then reply to sender
 * with command status.
 * @param spam Module_Spam object
 * @param command Gotham_Citizen_Command incoming command
 */
void
queue_check(Module_Spam *spam, Gotham_Citizen_Command *command)
{
   Queue *q;
   Eina_Strbuf *b;
   Eina_List *l,
             *ln;
   const char *answer;

   DBG("spam[%p] command[%p]=%s", spam, command, command->name);

   answer = utils_atos(command->command, "\n");

   EINA_LIST_FOREACH_SAFE(spam->queue.l, l, ln, q)
     {
        if ((strcmp(q->command, command->name)) ||
            (command->citizen != q->to))
          continue;

        b = eina_strbuf_new();
        eina_strbuf_append_printf(b, "\nAnswer from %s :\n%s\n\n",
                                  q->to->jid, answer);
        gotham_citizen_send(q->from, eina_strbuf_string_get(b));
        eina_strbuf_free(b);

        DBG("Removing queue for %s", q->command);
        spam->queue.l = eina_list_remove(spam->queue.l, q);
        _queue_free(q);
     }
   free((char *)answer);
}

/**
 * @}
 */
