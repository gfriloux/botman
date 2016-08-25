#include "main.h"

Module_Notification_Conf_Group *
utils_group_find(Module_Notification *notification,
                 const char *name)
{
   Eina_List *l;
   Module_Notification_Conf_Group *group;

   EINA_LIST_FOREACH(notification->conf->groups, l, group)
     if (!strcmp(name, group->name)) return group;

   return NULL;
}

const char *
utils_user_find(Module_Notification_Conf_Group *group,
                const char *name)
{
   Eina_List *l;
   const char *username;

   EINA_LIST_FOREACH(group->users, l, username)
     if (!strcmp(username, name)) return username;

   return NULL;
}
