#include "main.h"

#define _WRITE_CONF() gotham_serialize_struct_to_file(notification->conf, MODULE_NOTIFICATION_CONF, (Gotham_Serialization_Function)Module_Notification_Conf_to_azy_value)
#define _IF_SEND(_a, _b)                                                       \
   do {                                                                        \
      if (_a)                                                                  \
        {                                                                      \
           gotham_command_send(command, _b);                                   \
           return;                                                             \
        }                                                                      \
   } while (0)

void
alfred_group_print(Module_Notification *notification,
                   Gotham_Citizen_Command *command)
{
   Eina_Strbuf *buf;
   Module_Notification_Conf_Group *group;
   const char *user,
              *s;
   Eina_List *l,
             *l2;

   buf = eina_strbuf_new();

   eina_strbuf_append(buf, "\n");
   EINA_LIST_FOREACH(notification->conf->groups, l, group)
     {
        eina_strbuf_append_printf(buf, "Group %s :\n", group->name);

        EINA_LIST_FOREACH(group->users, l2, user)
          eina_strbuf_append_printf(buf, "\t%s\n", user);

        eina_strbuf_append(buf, "\n");
     }

   s = eina_strbuf_string_get(buf);
   gotham_command_send(command, s);
   eina_strbuf_free(buf);
}

void
alfred_group_add(Module_Notification *notification,
                 Gotham_Citizen_Command *command)
{
   Module_Notification_Conf_Group *group;
   const char *name;
   Eina_List *l;

   if (!command->command[2]) return;

   name = command->command[2];
   EINA_LIST_FOREACH(notification->conf->groups, l, group)
     if (!strcmp(name, group->name)) goto exists;

   group = Module_Notification_Conf_Group_new();

   group->name = eina_stringshare_add(name);

   notification->conf->groups = eina_list_append(notification->conf->groups, group);

   _WRITE_CONF();

   gotham_command_send(command, "New group created");
   return;

exists:
   gotham_command_send(command, "This group already exists");
}

void
alfred_group_del(Module_Notification *notification,
                 Gotham_Citizen_Command *command)
{
   Module_Notification_Conf_Group *group;

   EINA_SAFETY_ON_NULL_GOTO(command->command[2], print_usage);

   group = utils_group_find(notification, command->command[2]);
   _IF_SEND(!group, "Group not found.");

   notification->conf->groups = eina_list_remove(notification->conf->groups, group);
   _WRITE_CONF();
   gotham_command_send(command, "Group deleted");
   return;

print_usage:
   gotham_command_send(command, "Incorrect usage, please see .help");
}

void
alfred_user_add(Module_Notification *notification,
                Gotham_Citizen_Command *command)
{
   Module_Notification_Conf_Group *group;
   const char *username;
   Gotham_Citizen *citizen;

   EINA_SAFETY_ON_NULL_GOTO(command->command[2], print_usage);
   EINA_SAFETY_ON_NULL_GOTO(command->command[3], print_usage);

   citizen = eina_hash_find(notification->gotham->citizens, command->command[3]);
   _IF_SEND(!citizen, "This user is not known to me.");

   group = utils_group_find(notification, command->command[2]);
   _IF_SEND(!group, "Group not found.");

   username = utils_user_find(group, command->command[3]);
   _IF_SEND(username, "User already exists in this group");

   username = eina_stringshare_add(command->command[3]);
   group->users = eina_list_append(group->users, username);
   _WRITE_CONF();
   gotham_command_send(command, "User added to group.");
   return;

print_usage:
   gotham_command_send(command, "Incorrect usage, please see .help");
}

void
alfred_user_del(Module_Notification *notification,
                Gotham_Citizen_Command *command)
{
   Module_Notification_Conf_Group *group;
   const char *username;

   EINA_SAFETY_ON_NULL_GOTO(command->command[2], print_usage);
   EINA_SAFETY_ON_NULL_GOTO(command->command[3], print_usage);

   group = utils_group_find(notification, command->command[2]);
   _IF_SEND(!group, "Group not found.");

   username = utils_user_find(group, command->command[3]);
   _IF_SEND(!username, "User not found.");

   group->users = eina_list_remove(group->users, username);
   eina_stringshare_del(username);
   _WRITE_CONF();
   gotham_command_send(command, "User deleted from group.");
   return;

print_usage:
   gotham_command_send(command, "Incorrect usage, please see .help");
}

void
alfred_send(Module_Notification *notification,
            Gotham_Citizen_Command *command)
{
   Module_Notification_Conf_Group *group;
   unsigned int i;
   const char *user,
              *match;
   Eina_Strbuf *buf;
   Eina_List *l;

   EINA_SAFETY_ON_NULL_GOTO(command->command[2], print_usage);
   EINA_SAFETY_ON_NULL_GOTO(command->command[3], print_usage);

   group = utils_group_find(notification, command->command[2]);
   _IF_SEND(!group, "Group not found.");

   buf = eina_strbuf_new();

   match = gotham_citizen_match_print(notification->conf->vars,
                                      command->citizen,
                                      EINA_FALSE, EINA_FALSE);

   eina_strbuf_append_printf(buf, "Notification from %s : ", match);

   for (i = 3; command->command[i]; i++)
     eina_strbuf_append_printf(buf, "%s%s", i == 3 ? "" : " ", command->command[i]);

   EINA_LIST_FOREACH(group->users, l, user)
     {
        Gotham_Citizen *citizen;

        citizen = eina_hash_find(notification->gotham->citizens, user);
        if (citizen) gotham_citizen_send(citizen, eina_strbuf_string_get(buf));
     }

   eina_strbuf_free(buf);
   return;

print_usage:
   gotham_command_send(command, "Incorrect usage, please see .help");
}
