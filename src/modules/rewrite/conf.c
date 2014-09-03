#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Gotham.h>
#include <cJSON.h>
#include "rewrite.h"

/**
 * @addtogroup Gotham_Module_Rewrite
 * @{
 */

/**
 * @brief Load conf file into Module_Rewrite object.
 * @param rewrite Module_Rewrite object
 */
void
conf_load(Module_Rewrite *rewrite)
{
   cJSON *j,
         *ja;
   int s,
       i;
   Module_Rewrite_Rule *rule;

   DBG("rewrite[%p]", rewrite);

   j = gotham_modules_conf_load(MODULE_REWRITE_CONF);
   if (!j)
     return;

   ja = cJSON_GetObjectItem(j, "rules");
   if (!ja)
     {
        cJSON_Delete(j);
        return;
     }

   s = cJSON_GetArraySize(ja);
   for (i=0; i<s; i++)
     {
        cJSON *ja_r = NULL,
              *ja_r_n = NULL,
              *ja_r_f = NULL,
              *ja_r_r = NULL,
              *ja_r_d = NULL;

        ja_r = cJSON_GetArrayItem(ja, i);
        if (!rule)
          {
             cJSON_Delete(j);
             return;
          }

        ja_r_n = cJSON_GetObjectItem(ja_r, "name");
        ja_r_f = cJSON_GetObjectItem(ja_r, "filter");
        ja_r_r = cJSON_GetObjectItem(ja_r, "rule");
        ja_r_d = cJSON_GetObjectItem(ja_r, "description");
        if (!ja_r_n  || !ja_r_f || !ja_r_r || !ja_r_d)
          continue;

        rule = calloc(1, sizeof(Module_Rewrite_Rule));
        rule->name = strdup(ja_r_n->valuestring);
        rule->filter = strdup(ja_r_f->valuestring);
        rule->rule = eina_stringshare_add(ja_r_r->valuestring);
        rule->desc = strdup(ja_r_d->valuestring);

        DBG("rule[%p]=%s", rule, rule->name);

        rewrite->rules = eina_inlist_append(rewrite->rules,
                                            EINA_INLIST_GET(rule));
     }

   DBG("loaded %i rule%s", s, (s!=1) ? "s" : "");

   cJSON_Delete(j);
}

/**
 * @}
 */
