# Info Module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

## Description

This module plays with registered custom variables.\n
When run by botman, this module will periodically collect the
list of custom variables, and send them to alfred so he can register them
for each botman. This allows other modules running on Alfred to
have them synced and reuse them.\n\n

For example, a module might collect system inforations, and register
them as custom variables.\n
Custom variables being sent to alfred, it would be possible to search
for them using alfred.

## Configuration

### Alfred

You can add `custom variables` to be searchable as `<pattern>`.\n
These variables must be referenced inside the configuration
file (`/usr/local/etc/gotham/modules.conf.d/info.conf`) :
<pre>
{
   "search_vars" : [
      "variable_name"
   ]
}
</pre>

## Example use

### Get the list of `custom variables`

<pre>
<span style="color:blue">(11:35:51) guillaume.friloux@botnet.master.com:</span> .info
<span style="color:red">(11:35:51) botman-xxxx:</span> {
   "command":  ".info",
   "parameters":  "",
   "status":   "ok",
   "content":  [
      "version_ecore : 1.7.11_3",
      "version_kernel : 10.1-RELEASE",
      "tunnel_port : 0",
      "version_eet : 1.7.11_1",
      "zpool_status : DEGRADED",
      "version_eio : 1.7.11_3",
      "tunnel_pid : 0",
      "version_botman : 1.2.10_3",
      "sysinfo_serial_number : 928PR4J",
      "sysinfo_manufacturer : Dell Inc.",
      "sysinfo_product_name : OptiPlex 780",
      "version_maelstrom : 0.0.99_6",
      "version_eina : 1.7.11_1"
   ]
}
</pre>
\n\n

### Query a specific `custom variable`

<pre>
<span style="color:blue">(11:31:31) guillaume.friloux@botnet.master.com:</span> .info get zpool_status
<span style="color:red">(11:31:31) botman-xxxx:</span> zpool_status : DEGRADED
</pre>

