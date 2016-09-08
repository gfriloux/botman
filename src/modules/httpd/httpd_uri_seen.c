#include "httpd.h"

typedef Eina_List * (*Seen_Func)(Gotham *gotham, const char *pattern, Gotham_Citizen_Type citizen_type, void *data);

typedef struct _Httpd_Uri_Seen
{
   Seen_Func func;
   void *data;
} Httpd_Uri_Seen;

Eina_List *
_httpd_uri_seen_parse(Eina_List *l)
{
   Eina_List *l2;
   Eina_List *result = NULL;
   Gotham_Citizen *gc;

   EINA_LIST_FOREACH(l, l2, gc)
     {
        Httpd_Seen_Contact *hsc;
        Eina_Iterator *it;
        void *data;

        hsc = Httpd_Seen_Contact_new();

        hsc->jid = eina_stringshare_ref(gc->jid);

        hsc->state = eina_stringshare_add(gc->status == GOTHAM_CITIZEN_STATUS_OFFLINE ? "offline" : "online");

        it = eina_hash_iterator_tuple_new(gc->vars);
        while (eina_iterator_next(it, &data))
          {
             Httpd_Seen_Contact_Var *hscv;
             Eina_Hash_Tuple *t = data;
             const char *name = t->key;
             const char *value = t->data;

             hscv = Httpd_Seen_Contact_Var_new();

             hscv->name = eina_stringshare_add(name);
             hscv->value = eina_stringshare_add(value);

             hsc->vars = eina_list_append(hsc->vars, hscv);
          }

        result = eina_list_append(result, hsc);
     }

   return result;
}

void
httpd_uri_seen_init(void)
{
   Httpd_Uri_Seen *hus;

   hus = calloc(1, sizeof(Httpd_Uri_Seen));
   EINA_SAFETY_ON_NULL_RETURN(hus);

   hus->func = gotham_modules_function_get("Seen", "seen_query_match");
   EINA_SAFETY_ON_NULL_GOTO(hus->func, free_hus);

   hus->data = gotham_modules_data_get("Seen");
   DBG("hus->func[%p] hus->data[%p]", hus->func, hus->data);

   httpd_module_add("/seen", httpd_uri_seen, hus,
                    "This command allows you to search for botmans.<br />"
                    "It uses JID or custom variables to match them.<br />"
                    "<br />"
                    "<div class=\"panel panel-primary\">"
                    "  <div class=\"panel-heading\">"
                    "     <h3 class=\"panel-title\">Example without pattern</h3>"
                    "  </div>"
                    "  <div class=\"panel-body blackback\">"
                    "    <span class=yellow><b>curl</b></span> <span class=red>-X</span><span class=green>GET</span> <span class=white>http://127.0.0.1:5128</span><span class=cyan>/seen</span>"
                    "  </div>"
                    "</div>"
                    "<br />"
                    "<div class=\"panel panel-primary\">"
                    "  <div class=\"panel-heading\">"
                    "     <h3 class=\"panel-title\">Example with pattern</h3>"
                    "  </div>"
                    "  <div class=\"panel-body blackback\">"
                    "    <span class=yellow><b>curl</b></span> <span class=red>-X</span><span class=green>GET</span> <span class=white>http://127.0.0.1:5128</span><span class=cyan>/seen/</span><span class=green>*backup*</span>"
                    "  </div>"
                    "</div>"
                    "<h3>Possible returned HTTP codes</h3>"
                    "  <ul>"
                    "    <li>200 : Query OK.</li>"
                    "    <li><span class=red>500 : HTTPd module encountered an internal error.</span></li>"
                    "  </ul>"
                    "<h3>Returned data</h3>"
                    "  Data returned will be in JSON format.<br />"
                    "  It will be an array of contacts matching your pattern"
                    "  <pre>"
                    "  [<br />"
                    "    {<br />"
                    "      \"jid\":   \"botman-bull@xmpp-test.asp64.lan\",<br />"
                    "      \"state\": \"online\",<br />"
                    "      \"vars\":  [<br />"
                    "        {<br />"
                    "          \"name\":  \"version_ecore\",<br />"
                    "          \"value\": \"1.7.12\"<br />"
                    "        },<br />"
                    "        ...<br />"
                    "      ]<br />"
                    "    },<br />"
                    "    ...<br />"
                    "  ]<br />"
                    "  </pre>"
                    );

   return;

free_hus:
   free(hus);
}

void
httpd_uri_seen(void *seen_data,
               Azy_Server_Module *module,
               const char *uri,
               Azy_Net_Data *data EINA_UNUSED)
{
   Httpd_Uri_Seen *hus = seen_data;
   const char *s,
              *p;
   char **params;
   unsigned int nb;
   Eina_List *l,
             *result;
   Eina_Value *ev;

   params = eina_str_split_full(uri, "/", 3, &nb);
   EINA_SAFETY_ON_NULL_GOTO(params, error);

   p = (params[2] && params[2][0]) ? params[2] : "*";

   l = hus->func(_httpd->gotham, p, GOTHAM_CITIZEN_TYPE_BOTMAN, hus->data);

   result = _httpd_uri_seen_parse(l);

   ev = Array_Httpd_Seen_Contact_to_azy_value(result);
   EINA_SAFETY_ON_NULL_GOTO(ev, internal_free_result);

   s = azy_content_serialize_json(ev);
   eina_value_free(ev);
   EINA_SAFETY_ON_NULL_GOTO(s, internal_free_result);

   httpd_uri_data(module, (char *)s, strlen(s), 200);
   free((char *)s);
   return;

internal_free_result:
   eina_list_free(result);
   eina_list_free(l);
   free(params[0]);
   free(params);
error:
   httpd_uri_data(module, NULL, 0, 500);
}
