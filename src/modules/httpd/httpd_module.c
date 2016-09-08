#include "httpd.h"

void
httpd_module_add(const char *uri,
                 Httpd_Uri_Func func,
                 void *data,
                 const char *description)
{
   Module_Httpd_Module *mhm;

   mhm = calloc(1, sizeof(Module_Httpd_Module));
   EINA_SAFETY_ON_NULL_RETURN(mhm);

   mhm->uri = strdup(uri);
   EINA_SAFETY_ON_NULL_GOTO(mhm->uri, free_mhm);

   mhm->description = strdup(description);
   EINA_SAFETY_ON_NULL_GOTO(mhm->description, free_uri);

   mhm->cb.func = func;
   mhm->cb.data = data;

   _httpd->modules = eina_list_append(_httpd->modules, mhm);

   return;

free_uri:
   free((char *)mhm->uri);
free_mhm:
   free(mhm);
   return;
}

void
httpd_module_free(Module_Httpd_Module *mhm)
{
   free((char *)mhm->uri);
   free((char *)mhm->description);

   free(mhm);
}
