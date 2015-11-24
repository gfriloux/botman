# Sysinfo Module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

## Description

This module fetch various system informations, and stores them on alfred.
\n\n

## Configuration

### Alfred

<pre>
{
   "vars" : [
      "raid_status",
      "disk_full"
   ]
}
</pre>
The `vars` array contains the list of `custom variables` to be shown when
the `.sysinfo` command will list matching bots.

### Botman

<pre>
{
   "hw" : [{
      "manufacturer" : "/usr/local/sbin/dmidecode -t system | grep 'Manufacturer:' | cut -d: -f2",
      "product_name" : "/usr/local/sbin/dmidecode -t system | grep 'Product Name:' | cut -d: -f2",
      "serial_number" : "/usr/local/sbin/dmidecode -t system | grep 'Serial Number:' | cut -d: -f2"
   }],
   "commands" : [{
      ".uptime" : "/usr/bin/uptime",
      ".psax" : "/bin/ps ax",
      ".reboot" : "/sbin/reboot"
   }]
}
</pre>

\n\n


## Example use

### List system infos of a particular bot

<span class="label label-success">Alfred</span>
> <span style="color:blue">(09:49:42) guillaume.friloux@botnet.master.com:</span> .sysinfo *xxx*\n
> <span style="color:red">(09:49:42) Alfred Pennyworth:</span>\n
> Contacts that matched :\n
> &nbsp;&nbsp;&nbsp;online botman-xxx@botnet.master.com\n
> \n
> online botman-xxx@botnet.master.com  :\n
> &nbsp;&nbsp;&nbsp;manufacturer : Dell Inc.\n
> &nbsp;&nbsp;&nbsp;serial_number : XXXXX\n
> &nbsp;&nbsp;&nbsp;product_name : OptiPlex 780\n

