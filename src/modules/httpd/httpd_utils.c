#include "httpd.h"

char *
httpd_utils_file_get(const char *file,
                     size_t len)
{
   Eina_Strbuf *buf;
   char *s;

   DBG("Requested file %.*s (%zu)", (int)len, file, len);

   buf = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(buf, NULL);

   eina_strbuf_append(buf, DATA_DIR"/gotham/modules.d/httpd/www/");
   if (!strcmp(file, "/")) eina_strbuf_append(buf, "index.html");
   else eina_strbuf_append_length(buf, file + 1, len - 1);

   s = eina_strbuf_string_steal(buf);
   DBG("Serving file [%s]", s);
   eina_strbuf_free(buf);
   return s;
}

/*
 * Highly inefficient mime-type detection, but should be enough
 * for our use case.
 */
char *
httpd_utils_content_type(const char *file)
{
   char *p;
   unsigned int i;
   struct _Cmp
   {
      char *extension,
           *mimetype;
   };
   struct _Cmp _lkt[] = {
      { ".png" , "image/png"              },
      { ".gif" , "image/gif"              },
      { ".jpeg", "image/jpeg"             },
      { ".jpg" , "image/jpeg"             },
      { ".json", "application/json"       },
      { ".js"  , "application/javascript" },
      { ".html", "text/html"              },
      { ".htm" , "text/html"              },
      { ".css" , "text/css"               },
      { NULL   , NULL                     }
   };

   p = strrchr(file, '.');
   if (!p) goto end;

   for (i = 0; _lkt[i].extension; i++)
     {
        if (strcmp(p, _lkt[i].extension))
          continue;

        return strdup(_lkt[i].mimetype);
     }

end:
   return strdup("text/plain");
}

void
httpd_utils_send(Azy_Server_Module *module,
                 Azy_Net *net,
                 unsigned char *s,
                 size_t len,
                 const char *mimetype)
{
   Azy_Net_Data data;

   data.data = s;
   data.size = len;

   azy_net_code_set(net, 200);
   azy_net_header_set(net, "Access-Control-Allow-Origin", "*");
   azy_net_header_set(net, "Content-Type", mimetype);
   azy_server_module_send(module, net, &data);
}
