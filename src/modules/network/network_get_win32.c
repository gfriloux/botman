#ifdef _WIN32
#include "network.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

void
_network_get_win32_string_convert(const char **s)
{
   char *e,
        *stringshared;

   e = eina_str_convert("iso-8859-15", "utf-8", *s);
   *s = eina_stringshare_add(e);
   free(e);
}

Eina_List *
_network_get_win32_struct(void)
{
   PIP_ADAPTER_ADDRESSES paa,
                         p;
   ULONG paa_len = 0;
   DWORD r;
   unsigned int i;
   Eina_List *adapters = NULL;

   i = 0;
   do {
      paa = (IP_ADAPTER_ADDRESSES *) MALLOC(paa_len);
      EINA_SAFETY_ON_NULL_RETURN_VAL(paa, NULL);

        r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, paa, &paa_len);
        if (r != ERROR_BUFFER_OVERFLOW) break;

        FREE(paa);

        i++;

    } while ((r == ERROR_BUFFER_OVERFLOW) && (i < 3));

   EINA_SAFETY_ON_TRUE_RETURN_VAL(r != NO_ERROR, NULL);

   p = paa;

   while (p)
     {
        PIP_ADAPTER_UNICAST_ADDRESS p_unicast;
        PIP_ADAPTER_DNS_SERVER_ADDRESS p_dns_server;
        WCHAR ip[256] = L"";
        DWORD ip_len = sizeof(ip);
        char mac[20] = {0},
             *s;
        Network_Configuration_Adapter *nca;

        DBG("Found Adapter AdapterName[%s] Description[%S] FriendlyName[%S] DnsSuffix[%S]",
            p->AdapterName, p->Description, p->FriendlyName, p->DnsSuffix);

        s = mac;
        for (i = 0; i < (unsigned int) p->PhysicalAddressLength; i++)
          {
             sprintf(s, "%.2X:", (int) p->PhysicalAddress[i]);
             s += 3;
          }

        *(s-1) = 0;

        nca = Network_Configuration_Adapter_new();
        EINA_SAFETY_ON_NULL_GOTO(nca, end);

        nca->mtu = p->Mtu;

        nca->name = eina_stringshare_add(p->AdapterName);
        EINA_SAFETY_ON_NULL_GOTO(nca->name, free_nca);

        nca->name_friendly = eina_stringshare_printf("%S", p->FriendlyName);
        EINA_SAFETY_ON_NULL_GOTO(nca->name_friendly, free_nca);

        _network_get_win32_string_convert(&nca->name_friendly);

        nca->description = eina_stringshare_printf("%S", p->Description);
        EINA_SAFETY_ON_NULL_GOTO(nca->description, free_nca);

        _network_get_win32_string_convert(&nca->description);

        if (p->DnsSuffix) nca->dns_suffix = eina_stringshare_printf("%S", p->DnsSuffix);

        /* Seems %S might fail if p->DnsSuffix is an empty string ... */
        if (!nca->dns_suffix) nca->dns_suffix = eina_stringshare_add("");
        EINA_SAFETY_ON_NULL_GOTO(nca->dns_suffix, free_nca);

        nca->addresses = Network_Configuration_Adapter_Addresses_new();
        EINA_SAFETY_ON_NULL_GOTO(nca->addresses, free_nca);

        nca->addresses->unicast = NULL;
        nca->addresses->dns_server = NULL;

        nca->addresses->physical = eina_stringshare_add(strlen(mac) ? mac : "");

        for (i = 0, p_unicast = p->FirstUnicastAddress; p_unicast != NULL; i++, p_unicast = p_unicast->Next)
          {
             WSAAddressToString(p_unicast->Address.lpSockaddr, p_unicast->Address.iSockaddrLength,
                                NULL, (LPSTR)ip, &ip_len);
             nca->addresses->unicast = eina_list_append(nca->addresses->unicast, eina_stringshare_printf("%s", ip));
          }

        p_dns_server = p->FirstDnsServerAddress;
        for (i = 0, p_dns_server = p->FirstDnsServerAddress; p_dns_server != NULL; i++, p_dns_server = p_dns_server->Next)
          {
            WSAAddressToString(p_dns_server->Address.lpSockaddr, p_dns_server->Address.iSockaddrLength,
                               NULL, (LPSTR)ip, &ip_len);
            nca->addresses->dns_server = eina_list_append(nca->addresses->dns_server, eina_stringshare_printf("%s", ip));
          }

        adapters = eina_list_append(adapters, nca);

end:
        p = p->Next;
        continue;

free_nca:
        Network_Configuration_Adapter_free(nca);
        p = p->Next;
     }

   FREE(paa);
   return adapters;
}

const char *
network_get_win32_json(void)
{
   Eina_List *adapters;
   Eina_Value *ev;
   char *s;

   adapters = _network_get_win32_struct();
   EINA_SAFETY_ON_NULL_RETURN_VAL(adapters, NULL);

   ev = Array_Network_Configuration_Adapter_to_azy_value(adapters);
   EINA_SAFETY_ON_NULL_GOTO(ev, free_adapters);

   s = azy_content_serialize_json(ev);
   eina_value_free(ev);
   Array_Network_Configuration_Adapter_free(adapters);
   EINA_SAFETY_ON_NULL_RETURN_VAL(s, NULL);
   return s;

free_adapters:
   Array_Network_Configuration_Adapter_free(adapters);
   return NULL;
}

void
network_get_win32(Module_Network *network EINA_UNUSED,
                  Gotham_Citizen_Command *command)
{
   Eina_List *adapters;
   Network_Configuration_Adapter *nca;
   char *s;

   s = network_get_win32_json();
   EINA_SAFETY_ON_NULL_GOTO(s, error);

   _MSG(command, "", s);
   free(s);
   return;

error:
   _MSG(command, "", "Failed to fetch network configuration");
}
#endif
