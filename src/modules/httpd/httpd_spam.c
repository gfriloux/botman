#include "httpd.h"

typedef Eina_List *(*Spam_Func)(void *data, const char *pattern, const char *message);

typedef struct _Http_Spam
{
   Spam_Func func;
   void *data;
} Httpd_Spam;

Httpd_Spam *_hs;

void
httpd_spam_init(void)
{
   Httpd_Spam *hs;

   hs = calloc(1, sizeof(Httpd_Spam));
   EINA_SAFETY_ON_NULL_RETURN(hs);

   hs->func = gotham_modules_function_get("Spam", "spam_query");
   EINA_SAFETY_ON_NULL_GOTO(hs->func, free_hs);

   hs->data = gotham_modules_data_get("Spam");

   DBG("hs->func[%p] hs->data[%p]", hs->func, hs->data);

   _hs = hs;

   return;

free_hs:
   free(hs);
}

Eina_Bool
_httpd_spam_timeout(void *data)
{
   Module_Httpd_Queue *mhq = data;

   mhq->cb.func(mhq->cb.data, QUEUE_INCOMPLETE, mhq->bots);

   httpd_queue_remove(mhq);
   httpd_queue_free(mhq);
   return EINA_FALSE;
}

const char *
httpd_uri_spam_answer_to_json(Eina_List *answers)
{
   cJSON *json;
   Eina_List *l;
   char *s;
   Module_Httpd_Queue_Message *mhqm;

   json = cJSON_CreateArray();

   EINA_LIST_FOREACH(answers, l, mhqm)
     {
        cJSON *answer;

        answer = cJSON_CreateObject();

        cJSON_AddItemToObject(answer, "jid", cJSON_CreateString(mhqm->jid));
        cJSON_AddItemToObject(answer, "message",
                              cJSON_CreateString(mhqm->message ? mhqm->message : ""));

        cJSON_AddItemToArray(json, answer);
     }

   s = cJSON_Print(json);
   cJSON_Delete(json);

   return s;
}

Eina_Bool
httpd_spam_new(const char *pattern,
               const char *command,
               const char *message,
               Httpd_Queue_Cb done_cb,
               void *data)
{
   Eina_List *bots;
   Gotham_Citizen *citizen;
   Eina_List *l;
   Module_Httpd_Queue *mhq;

   bots = _hs->func(_hs->data, pattern, message);

   mhq = calloc(1, sizeof(Module_Httpd_Queue));
   EINA_SAFETY_ON_NULL_GOTO(mhq, free_bots);

   mhq->command = strdup(command);
   EINA_SAFETY_ON_NULL_GOTO(mhq->command, free_mhq);

   DBG("New queue item for command [%s]", command);

   mhq->timeout = ecore_timer_add(10.0, _httpd_spam_timeout, mhq);

   mhq->cb.func = done_cb;
   mhq->cb.data = data;

   EINA_LIST_FOREACH(bots, l, citizen)
     {
        Module_Httpd_Queue_Message *mhqm;

        if (citizen->status != GOTHAM_CITIZEN_STATUS_ONLINE)
          continue;

        DBG("We should add [%s] for command [%s]", citizen->jid, command);

        mhqm = calloc(1, sizeof(Module_Httpd_Queue_Message));
        EINA_SAFETY_ON_NULL_GOTO(mhqm, end);

        mhqm->jid = strdup(citizen->jid);
        EINA_SAFETY_ON_NULL_GOTO(mhqm->jid, free_mhqm);

        DBG("Adding [%s] to queue", mhqm->jid);

        mhq->bots = eina_list_append(mhq->bots, mhqm);

end:
        continue;

free_mhqm:
        free(mhqm);
     }

   if (!mhq->bots)
     {
        done_cb(data, QUEUE_NO_RESULTS, NULL);
        httpd_queue_free(mhq);
        return EINA_TRUE;
     }

   httpd_queue_add(mhq);

   eina_list_free(bots);
   return EINA_TRUE;

free_mhq:
   free(mhq);
free_bots:
   eina_list_free(bots);
   return EINA_FALSE;
}
