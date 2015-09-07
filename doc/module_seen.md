# Seen Module

<span class="label label-success">Alfred</span>

## Description

This module allows to search for bots matching a given pattern.\n
These patterns, by default, will be its `JID`. Luckilly it is extendable to what we call
`custom variables`.\n
You can make a module that will register `custom variables` (for example in the sysinfo module),
and then adding them in the configuration file of the seen module to be able to make
searches over their values.\n

This could allow you to search for `degraded raids`, `ip blocs`, `global health status`, whatever you have in mind.\n


## Configuration

This module can work with an empty configuration.\n
A valid empty configuration file (`/usr/local/etc/gotham/modules.conf.d/seen.conf`) is :
<pre>
{
   "vars" : [
   ]
}
</pre>
\n\n

If you need to add `custom variables` to your seen queries, you can add them this way :
<pre>
{
   "vars" : [
      "raid_status",
      "disk_full"
   ]
}
</pre>
\n\n

## Example use
<img src=data/img/seen_example.png>
In this example i searched for `botman-k*` which is every `JID` that
starts with the string `botman-k`.

\n\n
The search output is defined as follows :
- One line per matching botman.
- We firstly show if botman is or isnt connected to the XMPP server.
- The JID (first searchable term) of the botman.
- All other searchable terms (`custom variables`).
- If contact is offline, elapsed time since he went offline.
