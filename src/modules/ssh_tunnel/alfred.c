#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>
#include <ctype.h>

#include "ssh_tunnel.h"

/**
 * @addtogroup Gotham_Module_Ssh_Tunnel
 * @{
 */

/**
 * @brief Sort 2 citizen by tunnel.
 * @param data1 1st data to sort
 * @param data2 2nd data to sort
 * @return result of strcmp function
 */
int
_ssh_tunnel_sort(void *data1, void *data2)
{
   char *var1,
        *var2;
   const char *tmp;
   int ret;

   tmp = gotham_citizen_var_get(data1, "tunnel_port");
   var1 = strdup(tmp ? tmp : "0");

   tmp = gotham_citizen_var_get(data2, "tunnel_port");
   var2 = strdup(tmp ? tmp : "0");

   ret = strcmp(var1, var2);
   free(var1);
   free(var2);
   return ret;
}

/**
 * @brief Add a line to result ("citizen that match" part).
 * When a citizen matches the search pattern, add a line to result
 * with citizen and some informations : status (on/offline), softwares
 * licence number, bunker.
 * @param obj Module object
 * @param citizen Gotham_Citizen to print
 * @return const char * line to add to result
 */
const char *
_citizen_match_print(Module_Ssh_Tunnel *obj,
                     Gotham_Citizen *citizen)
{
   Eina_Strbuf *buf;
   Eina_Array_Iterator it;
   unsigned int i;
   const char *item,
              *ptr;

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "%s %s ",
                             (citizen->status==GOTHAM_CITIZEN_STATUS_OFFLINE) ?
                                "offline" : "online",
                             citizen->jid);

   EINA_ARRAY_ITER_NEXT(obj->vars, i, item, it)
     {
        const char *var = VARGET(item);

        if (!var)
          continue;

        eina_strbuf_append_printf(buf, "%s[%s] ", item, var);
     }

   ptr = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return ptr;
}

/**
 * @brief Show stored tunnel ports for a pattern.
 * For each citizen that match pattern, get the tunnel port
 * from custom vars and send result.
 * @param obj Module object
 * @param command Gotham_Citizen_Command containing pattern to show
 */
void
alfred_command_tunnels_show(Module_Ssh_Tunnel *obj,
                            Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;
   Eina_Bool found = EINA_FALSE;
   Eina_Iterator *it;
   void *data;

   DBG("obj[%p], command[%p]", obj, command);

   EINA_SAFETY_ON_NULL_RETURN(obj);
   EINA_SAFETY_ON_NULL_RETURN(command);

   buf = eina_strbuf_new();
   eina_strbuf_append_printf(buf, "\nContacts with an opened tunnel :\n");

   /* Run through citizen list and find those who have an opened tunnel */
   it = eina_hash_iterator_data_new(obj->gotham->citizens);

   while(eina_iterator_next(it, &data))
     {
        Gotham_Citizen *citizen = data;
        const char *line,
                   *var;

        var = VARGET("tunnel_port");
        if (!var || (var[0] == '0'))
          continue;

        found = EINA_TRUE;

        line = _citizen_match_print(obj, citizen);
        eina_strbuf_append_printf(buf, "%s : Port %s\n", line, var);
        free((char *)line);
     }
   eina_iterator_free(it);

   gotham_command_send(command,
                       (found) ? eina_strbuf_string_get(buf) :
                                 "No one has an opened tunnel");

   eina_strbuf_free(buf);
   return;
}


/**
 * @brief Used for Alfred to get a Botman's answer (JSON).
 * This answer contains the tunnel port if one is opened.
 * It will be stored in a custom var.
 * @param obj Module object
 * @param command Gotham_Citizen_Command for botman answer
 */
void
botman_answer_get(Module_Ssh_Tunnel *obj,
                  Gotham_Citizen_Command *command)
{
   Gotham_Citizen *citizen = command->citizen;
   const char **msg;
   unsigned int tunnel_port = 0;

   EINA_SAFETY_ON_NULL_RETURN(obj);
   EINA_SAFETY_ON_NULL_RETURN(command);

   DBG("obj[%p] command[%p]=%s", obj, command, command->name);

   if (!command->command[0])
     {
        ERR("No content in botman's message");
        return;
     }

   DBG("Botman's message : %s", command->command[0]);

   /* Split message into array */
   msg = gotham_utils_msgtocommand(command->command[0]);

   /* If answer is "No tunnel found" => set port to 0 */
   if ((msg[0]) && (!strncmp(msg[0], "No", 2)))
     goto func_end;
   /* If answer is "Tunnel has died" => set port to 0 */
   else if ((msg[1]) && (!strncmp(msg[1], "has", 3)))
     goto func_end;
   /* Else : get port number and set it */
   else if ((msg[1]) &&
       (!strncmp(msg[1], "opened", 6)) &&
       (msg[4]))
     {
        char *tmp = strdup(msg[4]);

        if (tmp[strlen(tmp)-1] == ',')
           tmp[strlen(tmp)-1] = 0;

        tunnel_port = atoi(tmp);
        free(tmp);

        goto func_end;
     }
   else
     {
        ERR("Unparsable tunnel message !");
     }

func_end:
   DBG("Setting tunnel_port to %d", tunnel_port);
   VARSET("tunnel_port", "%d", tunnel_port);
   free((char *)msg[0]);
   free(msg);
   return;
}

/**
 * @}
 */
