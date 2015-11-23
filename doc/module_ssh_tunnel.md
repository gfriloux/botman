# SSH Tunnel Module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

## Description

This module will make the botman to start an SSH Tunnel.\n
This may allow you to get an ssh access to a NATed bot.\n
\n\n

## Configuration

### Alfred

You need a configuration file (`/usr/local/etc/gotham/modules.conf.d/ssh_tunnel.conf`)
<pre>
{
   "vars" : [
      "raid_status",
      "disk_full"
   ]
}
</pre>
\n
The `vars` array contains the list of `custom variables` the will be shown
when showing bots affected by the `.ssh` commands.

\n\n

### Botman

You need a configuration file (`/usr/local/etc/gotham/modules.conf.d/ssh_tunnel.conf`)
<pre>
{
   "host" : "ssh.master.com",
   "login" : "support",
   "key" : "/usr/local/share/gotham/modules.d/ssh_tunnel/id_rsa_support",
   "port" : 22
}
</pre>

This file contains informations needed to start a reverse ssh tunnel
on the `host`.

\n\n

## Example use

### List openned tunnels

<span class="label label-success">Alfred</span>
> <span style="color:blue">(16:27:43) guillaume.friloux@botman.master.com:</span> .ssh\n
> <span style="color:red">(16:27:43) Alfred Pennyworth:</span>\n
> Contacts with an opened tunnel :\n
> &nbsp;&nbsp;&nbsp;online botman.1@botman.master.com  : Port 42461\n
> &nbsp;&nbsp;&nbsp;online botman.2@botman.master.com  : Port 57887\n
\n

### Open SSH tunnel

<span class="label label-primary">Botman</span>
> <span style="color:blue">(16:41:18) guillaume.friloux@botman.master.com:</span> .ssh on\n
> <span style="color:red">(16:41:18) botman:</span> Tunnel opened on port 58462, pid 37669\n
\n

### Close SSH tunnel

<span class="label label-primary">Botman</span>
> <span style="color:blue">(16:43:37) guillaume.friloux@botman.master.com:</span> .ssh off\n
> <span style="color:red">(16:43:37) botman:</span> Tunnel has died\n
