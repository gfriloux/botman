#include "httpd.h"

Eina_List *_queue;

void
httpd_queue_init(void)
{
   _queue = NULL;
}

void
httpd_queue_remove(Module_Httpd_Queue *mhq)
{
   _queue = eina_list_remove(_queue, mhq);
}


void
httpd_queue_add(Module_Httpd_Queue *mhq)
{
   _queue = eina_list_append(_queue, mhq);
}

void
_httpd_queue_message_free(Module_Httpd_Queue_Message *mhqm)
{
   free((char *)mhqm->jid);
   free((char *)mhqm->message);
   free(mhqm);
}

void
httpd_queue_free(Module_Httpd_Queue *mhq)
{
   Module_Httpd_Queue_Message *mhqm;

   free((char *)mhq->command);

   if (mhq->timeout)
     {
        ecore_timer_del(mhq->timeout);
        mhq->timeout = NULL;
     }
   EINA_LIST_FREE(mhq->bots, mhqm)
     _httpd_queue_message_free(mhqm);

   free((char *)mhq);
}

void
_httpd_queue_complete(Module_Httpd_Queue *mhq)
{
   Module_Httpd_Queue_Message *mhqm;
   Eina_List *l;

   EINA_LIST_FOREACH(mhq->bots, l, mhqm)
     if (!mhqm->message) return;

   DBG("Complete !");
   mhq->cb.func(mhq->cb.data, QUEUE_COMPLETE, mhq->bots);

   _queue = eina_list_remove(_queue, mhq);
   httpd_queue_free(mhq);
}

char *
_httpd_queue_perform_stringify(const char **lines)
{
   Eina_Strbuf *buf;
   char *s;
   unsigned int i = 0;

   buf = eina_strbuf_new();

   goto next;

   for (; lines[i]; i++)
     {
        eina_strbuf_append(buf, "\n");
next:
        eina_strbuf_append(buf, lines[i]);
     }

   s = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return s;
}

void
httpd_queue_perform(Gotham_Citizen_Command *command)
{
   Module_Httpd_Queue *mhq;
   Module_Httpd_Queue_Message *mhqm;
   Eina_List *l,
             *l2;
   char *s;

   DBG("command->name[%s]", command->name);

   DBG("command->command[0][%s]", command->command[0]);

   EINA_LIST_FOREACH(_queue, l, mhq)
     {
        DBG("Queue for command [%s]", mhq->command);

        if (strcmp(mhq->command, command->name))
          continue;

        EINA_LIST_FOREACH(mhq->bots, l2, mhqm)
           {
              if (mhqm->message) continue;

              DBG("command->jid[%s] <-> mhqm->jid[%s]", command->jid, mhqm->jid);

              if (strncmp(command->jid, mhqm->jid, strlen(mhqm->jid))) continue;

              s = _httpd_queue_perform_stringify(command->command);

              DBG("Adding message [%s]", s);
              mhqm->message = s;
              _httpd_queue_complete(mhq);
           }
     }
}
