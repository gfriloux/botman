#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <Ecore.h>
#include <ctype.h>
#include <math.h>

#include "version.h"

/**
 * @addtogroup Gotham_Module_Version
 * @{
 */

/**
 * @brief Sort two version n° on their name.
 * @param data1 Module_Version_Find data
 * @param data2 Module_Version_Find data
 * @return strcmp result between the two versions.
 */
int
_version_sort(void *data1, void *data2)
{
   Module_Version_Find *f1 = data1,
                       *f2 = data2;
   return strcmp(f1->version, f2->version);
}

/**
 * @brief Create a new Module_Version_Software object
 * and convert given version into this object.
 * @param version const char * version to convert
 * @return newly created object.
 */
Module_Version_Software *
_module_version_software_new(const char *version)
{
   Module_Version_Software *obj = NULL;
   char *tmp,
        **split_v,
        *p;
   unsigned int i;

   EINA_SAFETY_ON_NULL_RETURN_VAL(version, NULL);
   DBG("version[%p]=%s", version, version);

   obj = calloc(1, sizeof(Module_Version_Software));
   if (!obj)
     return NULL;

   obj->nb_p=obj->nb_v=0;

   tmp = strdup(version);

   p = strrchr(tmp, '-');
   if (p)
     {
        char **split_p;
        ++p;
        split_p = eina_str_split_full(p, "-", 0, &obj->nb_p);
        if (!split_p)
          goto end_p;

        obj->p = calloc(1, sizeof(unsigned int)*obj->nb_p);
        for (i=0; i<obj->nb_p; i++)
          obj->p[i] = atoi(split_p[i]);
        free(split_p[0]);
        free(split_p);
end_p:
        *p='\0';
     }

   split_v = eina_str_split_full(tmp, ".", 0, &obj->nb_v);
   if (!split_v)
     {
        ERR("Unable to split “%s” on “.”", tmp);
        goto error;
     }
   obj->v = calloc(1, sizeof(unsigned int)*obj->nb_v);
   for(i=0; i<obj->nb_v; i++)
     obj->v[i] = atoi(split_v[i]);

   free(split_v[0]);
   free(split_v);
   free(tmp);
   tmp = NULL;

   if ((!obj->nb_v) || ((obj->nb_v==1) && (obj->v[0]==0)))
     goto error;

   return obj;

error:
   free(tmp);
   free(obj);
   return NULL;
}

/**
 * @brief Free a Module_Version_Software object.
 * @param s Module_Version_Software object to free
 */
void
_module_version_software_free(Module_Version_Software *s)
{
   EINA_SAFETY_ON_NULL_RETURN(s);
   free(s->v);
   free(s->p);
   free(s);
}

/**
 * @brief Get the enum value(s) matching the given char * sign.
 * @param sign Sign to check
 * @return Enum corresponding to given sign
 */
Module_Version_Sign
_module_version_sign_get(const char *sign)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(sign, MODULE_VERSION_SIGN_UNIDENTIFIED);

   if (!strcmp(sign, "<")) return MODULE_VERSION_SIGN_LT;
   if (!strcmp(sign, "<=")) return MODULE_VERSION_SIGN_LT | MODULE_VERSION_SIGN_EQ;
   if (!strcmp(sign, "=")) return MODULE_VERSION_SIGN_EQ;
   if (!strcmp(sign, ">=")) return MODULE_VERSION_SIGN_GT | MODULE_VERSION_SIGN_EQ;
   if (!strcmp(sign, ">")) return MODULE_VERSION_SIGN_GT;
   if (!strcmp(sign, "!=")) return MODULE_VERSION_SIGN_DIFF;
   return MODULE_VERSION_SIGN_UNIDENTIFIED;
}

/**
 * @brief Compare two versions.
 * Compare each version component from "is" (software version and
 * package version) with the same components from "wanted"
 * and for each sub-value, store the equality in a result array.
 * (Notice that "3" will be equal to "3.0.0" but lower than "3.0.1")
 * We end with an array we can run through and define the global
 * equality.
 * Then we check if this final sign mathes the given sign, and return
 * EINA_TRUE or EINA_FALSE in consequence.
 * @param wanted Module_Version_Software version expected
 * @param sign Module_Version_Sign sign(s) to use for check
 * @param is Module_Version_Software version to check
 * @return EINA_TRUE if "is" matches requirements, EINA_FALSE otherwise
 */
Eina_Bool
_module_version_software_compare(Module_Version_Software *wanted,
                                 Module_Version_Sign sign,
                                 Module_Version_Software *is)
{
   unsigned int max_v,
                max_p,
                i;
   Module_Version_Sign final = MODULE_VERSION_SIGN_UNIDENTIFIED,
                       *res;

   /**
    * Determine the max elements for version nb and package rev.
    */
   max_v = (wanted->nb_v > is->nb_v) ? wanted->nb_v : is->nb_v;
   max_p = (wanted->nb_p > is->nb_p) ? wanted->nb_p : is->nb_p;
   res = calloc(max_v + max_p, sizeof(Module_Version_Sign));

#define _SOLVE(_a, _b, _c, _d, _e, _f, _g)                                     \
   do                                                                          \
   {                                                                           \
      unsigned int inc = _g;                                                   \
      for (i=0; i<_a; i++)                                                     \
        {                                                                      \
           unsigned int want_val,                                              \
                        is_val;                                                \
           want_val = (i < _b) ? _c[i] : 0;                                    \
           is_val = (i < _d) ? _e[i] : 0;                                      \
           DBG("[%d] want_val=%d is_val=%d", i, want_val, is_val);             \
           if (is_val == want_val)                                             \
             _f[i + inc] = MODULE_VERSION_SIGN_EQ;                             \
           if (is_val > want_val)                                              \
             _f[i + inc] = MODULE_VERSION_SIGN_GT;                             \
           if (is_val < want_val)                                              \
             _f[i + inc] = MODULE_VERSION_SIGN_LT;                             \
        }                                                                      \
   } while (0)
/* "debug" */
   _SOLVE(max_v, wanted->nb_v, wanted->v, is->nb_v, is->v, res, 0);
   _SOLVE(max_p, wanted->nb_p, wanted->p, is->nb_p, is->p, res, max_v);
#undef _SOLVE

   for (i=0; i<(max_v + max_p); i++)
     {
        final = res[i];
        DBG("[%d] final=%d", i, final);
        /* While equality is found, we go on */
        if (res[i] == MODULE_VERSION_SIGN_EQ)
          continue;
        /* On the first > or < sign, we break */
        break;
     }

   free(res);

#define _S(_a) MODULE_VERSION_SIGN_##_a

   if (((sign & _S(EQ)  ) && (final == _S(EQ))) ||
       ((sign & _S(LT)  ) && (final == _S(LT))) ||
       ((sign & _S(GT)  ) && (final == _S(GT))) ||
       ((sign & _S(DIFF)) && (final == _S(EQ))))
     return EINA_TRUE;
   return EINA_FALSE;
}


/**
 * @brief Run a .version find command and return results to sender.
 * For each citizen, get (from custom vars) the version n° matching
 * required software and make the comparison.
 * Then sort the final citizen by ascending version, and return result.
 * @param version Module object
 * @param command Gotham_Citizen_Command inconming command
 */
void
version_alfred_find_command(void *module_data,
                            Gotham_Citizen_Command *command)
{
   Module_Version *version = module_data;
   Gotham *gotham;
   const char **version_cmd = command->command;
   char *software,
        *current_version = NULL;
   Module_Version_Software *s_version;
   Module_Version_Sign sign;
   Module_Version_Find *data;

   Gotham_Citizen *citizen;
   Eina_Iterator *it;
   void *it_data;

   Eina_List *l_citizen = NULL;
   Eina_Strbuf *buf;
   Eina_Bool found = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN(version);
   EINA_SAFETY_ON_NULL_RETURN(command);

   gotham = version->gotham;

   if ((!version_cmd[2]) || (!version_cmd[3]) || (!version_cmd[4]))
     goto send_usage;

   software = dupf("version_%s", version_cmd[2]);
   if (!software)
     goto send_usage;

   eina_str_tolower(&software);

   sign = _module_version_sign_get(version_cmd[3]);
   if (sign == MODULE_VERSION_SIGN_UNIDENTIFIED)
     {
        ERR("Unrecognized compare sign “%s”", version_cmd[3]);
        goto free_software;
     }

   s_version = _module_version_software_new(version_cmd[4]);
   if (!s_version)
     {
        ERR("Unparsable version “%s”", version_cmd[4]);
        goto free_software;
     }

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "\nContacts that matched “%s%s%s” :\n",
                             version_cmd[2], version_cmd[3], version_cmd[4]);

   it = eina_hash_iterator_data_new(gotham->citizens);
   while (eina_iterator_next(it, &it_data))
     {
        const char *var;
        Eina_Bool match;
        Module_Version_Software *c_version;
        Module_Version_Find *find;
        citizen = it_data;

        if (citizen->type != GOTHAM_CITIZEN_TYPE_BOTMAN)
          continue;

        var = VARGET(software);
        if (!var)
          continue;

        c_version = _module_version_software_new((const char *)var);
        if (!c_version)
          {
             DBG("Unparsable version “%s”", var);
             continue;
          }

        /** Compare versions */
        match = _module_version_software_compare(s_version, sign, c_version);
        _module_version_software_free(c_version);
        if (!match)
          continue;

        DBG("%s %s matches", software, var);

        found = EINA_TRUE;

        find = calloc(1, sizeof(Module_Version_Find));
        find->version = strdup(var);
        find->citizen = citizen;
        l_citizen = eina_list_append(l_citizen, find);
     }
   eina_iterator_free(it);
   _module_version_software_free(s_version);
   free(software);

   l_citizen = eina_list_sort(l_citizen, 0, (Eina_Compare_Cb)_version_sort);

   EINA_LIST_FREE(l_citizen, data)
     {
        const char *line = gotham_citizen_match_print(version->vars, data->citizen, EINA_TRUE, EINA_FALSE);

        if ((!current_version) || (strcmp(current_version, data->version)))
          {
             free(current_version);
             current_version = strdup(data->version);
             eina_strbuf_append_printf(buf, "\n%s : %s\n",
                                       version_cmd[2], current_version);
          }
        eina_strbuf_append_printf(buf, "\t%s\n", line);
        free((char *)line);
        free((char *)data->version);
        free(data);
     }

   if (!found)
     eina_strbuf_append(buf, "\nNo one matches software version");

   DBG("Sending back to %s:\n%s",
       command->citizen->jid, eina_strbuf_string_get(buf));

   gotham_command_send(command, eina_strbuf_string_get(buf));

   eina_strbuf_free(buf);
   free(current_version);
   return;

free_software:
   free(software);
send_usage:
   gotham_command_send(command,
                       "Usage : .version find software sign version\n"
                       "(ie : .software find nano < 2.2.1)");
}

/**
 * @}
 */
