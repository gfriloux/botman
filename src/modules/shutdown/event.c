#include "shutdown.h"

#ifdef _WIN32
BOOL
_win32_shutdown(Gotham_Citizen_Command *command)
{
   HANDLE hToken;
   TOKEN_PRIVILEGES tkp;

   // Get a token for this process.
   if (!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
      return( FALSE );

   // Get the LUID for the shutdown privilege.
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
        &tkp.Privileges[0].Luid);

   tkp.PrivilegeCount = 1;  // one privilege to set
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

   // Get the shutdown privilege for this process.
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
        (PTOKEN_PRIVILEGES)NULL, 0);

   if (GetLastError() != ERROR_SUCCESS) return FALSE;

   // Shut down the system and force all applications to close.
   if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
               SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
               SHTDN_REASON_MINOR_UPGRADE |
               SHTDN_REASON_FLAG_PLANNED))
      return FALSE;

   //shutdown was successful
   return TRUE;
}
#endif


Eina_Bool
event_citizen_command(void *data EINA_UNUSED,
                      int type EINA_UNUSED,
                      void *ev)
{
   Gotham_Citizen_Command *command =ev;

   EINA_SAFETY_ON_NULL_RETURN_VAL(command, EINA_TRUE);

   DBG("command[%p][%s]", command, command->name);

   if (command->citizen->type == GOTHAM_CITIZEN_TYPE_BOTMAN)
     return EINA_TRUE;

   if (strcmp(command->name, ".reboot"))
     return EINA_TRUE;

   command->handled = EINA_TRUE;

#ifdef _WIN32
   {
      BOOL r;

      r = _win32_shutdown(command);
      if (!r)
        {
           char s[128];
           ERR("ExitWindowsEx() failed : %d", GetLastError());
           sprintf(s, "Failed to reboot OS : %d", GetLastError());
           _MSG(command, "reboot", s);
        }
      else _MSG(command, "reboot", "Reboot initiated");
   }
#endif

   return EINA_TRUE;
}
