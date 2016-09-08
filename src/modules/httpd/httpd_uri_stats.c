#include <sys/resource.h>

#include "httpd.h"

void
_httpd_uri_stats_last_queries_add(Httpd_Stats_Query *hsq)
{
   DBG("[%i] [%s] [%s]", hsq->timestamp, hsq->source, hsq->command);

   _httpd->stats.last_queries = eina_list_append(_httpd->stats.last_queries, hsq);
   if (eina_list_count(_httpd->stats.last_queries) > 20)
     {
        hsq = eina_list_nth(_httpd->stats.last_queries, 0);
        DBG("Deleting query [%s]", hsq->command);
        _httpd->stats.last_queries = eina_list_remove(_httpd->stats.last_queries, hsq);
        Httpd_Stats_Query_free(hsq);
     }
}

void
httpd_uri_stats_command_add(Gotham_Citizen_Command *command)
{
   Httpd_Stats_Query *hsq;

   hsq = Httpd_Stats_Query_new();
   EINA_SAFETY_ON_NULL_RETURN(hsq);

   hsq->source = eina_stringshare_add(command->citizen->jid);
   EINA_SAFETY_ON_NULL_GOTO(hsq->source, free_hsq);

   hsq->command = eina_stringshare_add(command->message);
   EINA_SAFETY_ON_NULL_GOTO(hsq->command, free_hsq);

   hsq->data = eina_stringshare_add("");
   EINA_SAFETY_ON_NULL_GOTO(hsq->data, free_hsq);

   time((time_t *)&hsq->timestamp);

   _httpd_uri_stats_last_queries_add(hsq);
   return;

free_hsq:
   Httpd_Stats_Query_free(hsq);
}

void
httpd_uri_stats_query_add(Azy_Server_Module *module,
                          const char *path,
                          Azy_Net_Data *data)
{
   Azy_Net *net;
   Httpd_Stats_Query *hsq;

   if (!strncmp(path, "/stats", 6)) return;

   net = azy_server_module_net_get(module);

   hsq = Httpd_Stats_Query_new();
   EINA_SAFETY_ON_NULL_RETURN(hsq);

   hsq->source = eina_stringshare_add(azy_net_ip_get(net));
   EINA_SAFETY_ON_NULL_GOTO(hsq->source, free_hsq);

   hsq->command = eina_stringshare_add(path);
   EINA_SAFETY_ON_NULL_GOTO(hsq->command, free_hsq);

   time((time_t *)&hsq->timestamp);

   if (data->size)
     hsq->data = eina_stringshare_printf("%.*s", (int)data->size, data->data);
   else hsq->data = eina_stringshare_add("");

   EINA_SAFETY_ON_NULL_GOTO(hsq->data, free_hsq);

   _httpd_uri_stats_last_queries_add(hsq);
   return;

free_hsq:
   Httpd_Stats_Query_free(hsq);
}

Eina_List *
_httpd_uri_stats_contacts_get(void)
{
   Eina_List *contacts = NULL;
   Eina_Iterator *it;
   void *data;

   it = eina_hash_iterator_tuple_new(_httpd->gotham->citizens);
   while (eina_iterator_next(it, &data))
     {
        Eina_Hash_Tuple *t = data;
        Gotham_Citizen *gc = t->data;
        Httpd_Stats_Contact *hsc;

        if (!strcmp(gc->jid, _httpd->gotham->me->jid)) continue;

        hsc = Httpd_Stats_Contact_new();

        hsc->jid = eina_stringshare_add(gc->jid);
        hsc->online = gc->status == GOTHAM_CITIZEN_STATUS_OFFLINE ? EINA_FALSE : EINA_TRUE;

        switch (gc->type)
          {
           case GOTHAM_CITIZEN_TYPE_BOTMAN:
             {
                hsc->type = eina_stringshare_add("Botman");
                break;
             }
           case GOTHAM_CITIZEN_TYPE_ALFRED:
             {
                hsc->type = eina_stringshare_add("Alfred");
                break;
             }
           case GOTHAM_CITIZEN_TYPE_CIVILIAN:
             {
                hsc->type = eina_stringshare_add("Civilian");
                break;
             }
           case GOTHAM_CITIZEN_TYPE_UNIDENTIFIED:
           default:
             {
                hsc->type = eina_stringshare_add("Unidentified");
             }
          }

        contacts = eina_list_append(contacts, hsc);
     }
   eina_iterator_free(it);

   return contacts;
}

long
_httpd_uri_stats_memory(void)
{
   struct rusage r_usage;

   getrusage(RUSAGE_SELF, &r_usage);

   return r_usage.ru_maxrss * 1024;
}

double
_httpd_uri_stats_uptime(void)
{
   time_t now;

   time(&now);

   return now - _httpd->stats.start;
}


void
httpd_uri_stats(void *memory_data EINA_UNUSED,
                Azy_Server_Module *module,
                const char *uri EINA_UNUSED,
                Azy_Net_Data *data EINA_UNUSED)
{
   Httpd_Stats *hs;
   Eina_Value *ev;
   char *s;

   hs = Httpd_Stats_new();
   EINA_SAFETY_ON_NULL_GOTO(hs, internal);

   hs->memory = Httpd_Stats_Memory_new();
   EINA_SAFETY_ON_NULL_GOTO(hs->memory, internal);

   hs->memory->used = _httpd_uri_stats_memory();
   hs->uptime = _httpd_uri_stats_uptime();

   hs->contacts = _httpd_uri_stats_contacts_get();

   hs->queries = Array_Httpd_Stats_Query_copy(_httpd->stats.last_queries);

   ev = Httpd_Stats_to_azy_value(hs);
   EINA_SAFETY_ON_NULL_GOTO(ev, internal_free_hs);

   s = azy_content_serialize_json(ev);
   eina_value_free(ev);
   EINA_SAFETY_ON_NULL_GOTO(s, internal_free_hs);

   httpd_uri_data(module, (char *)s, strlen(s), 200);
   free(s);
   Httpd_Stats_free(hs);
   return;

internal_free_hs:
   Httpd_Stats_free(hs);
internal:
   httpd_uri_data(module, NULL, 0, 500);
}



void
httpd_uri_stats_init(void)
{
   time(&_httpd->stats.start);

   httpd_module_add("/stats", httpd_uri_stats, NULL,
                    "This API call will give out various stats about "
                    "Alfred's process."
                    "<h3>Possible returned HTTP codes</h3>"
                    "  <ul>"
                    "    <li>200 : Query OK.</li>"
                    "    <li><span class=red>500 : HTTPd module encountered an internal error.</span></li>"
                    "  </ul>"
                    "<h3>Returned data</h3>"
                    "  Data returned will be in JSON format.<br />"
                    "  <pre>"
                    "  {<br />"
                    "     <span class=red>\"uptime\"</span>:   <span class=olivedrab>3</span>,<br />"
                    "     <span class=red>\"memory\"</span>:   {<br />"
                    "        <span class=red>\"used\"</span>:  <span class=olivedrab>64163840</span><br />"
                    "     },<br />"
                    "     <span class=red>\"contacts\"</span>: [{<br />"
                    "        <span class=red>\"jid\"</span>:   <span class=red>\"botman-testdev@xmpp-test.asp64.lan\"</span>,<br />"
                    "        <span class=red>\"type\"</span>:  <span class=red>\"Botman\"</span>,<br />"
                    "        <span class=red>\"online\"</span>:   <span class=darkblue>true</span><br />"
                    "     }],<br />"
                    "     <span class=red>\"queries\"</span>:  [{<br />"
                    "        <span class=red>\"ip\"</span>: <span class=red>\"127.0.0.1\"</span>,<br />"
                    "        <span class=red>\"uri\"</span>:   <span class=red>\"/seen\"</span>,<br />"
                    "        <span class=red>\"data\"</span>:  <span class=red>\"\"</span>,<br />"
                    "        <span class=red>\"timestamp\"</span>:   <span class=olivedrab>1463730596</span><br />"
                    "     }]<br />"
                    "  }"
                    "  </pre>"
                   );
}
