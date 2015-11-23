# Module Module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

## Description

This module allows to list, load and unload modules.\n
This module is mostly a POC, but could be used for example to update
an alfred module without restart alfred.
\n\n

## Configuration

There isnt any configuration to be done for this module.
\n\n

## Example use

### List modules

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>
> <span style="color:blue">(16:58:10) guillaume.friloux@botnet.master.com:</span> .module\n
> <span style="color:red">(16:58:10) botman:</span>\n
> Loaded modules :\n
> &nbsp;&nbsp;&nbsp;Version (/usr/local/lib/gotham/freebsd10.1-amd64/version.so)\n
> &nbsp;&nbsp;&nbsp;Install (/usr/local/lib/gotham/freebsd10.1-amd64/install.so)\n
> &nbsp;&nbsp;&nbsp;Info (/usr/local/lib/gotham/freebsd10.1-amd64/info.so)\n
> &nbsp;&nbsp;&nbsp;Alert (/usr/local/lib/gotham/freebsd10.1-amd64/alert.so)\n
> &nbsp;&nbsp;&nbsp;Module (/usr/local/lib/gotham/freebsd10.1-amd64/module.so)\n
> &nbsp;&nbsp;&nbsp;Ssh_Tunnel (/usr/local/lib/gotham/freebsd10.1-amd64/ssh_tunnel.so)\n
> &nbsp;&nbsp;&nbsp;Sysinfo (/usr/local/lib/gotham/freebsd10.1-amd64/sysinfo.so)\n
> &nbsp;&nbsp;&nbsp;Prosave_Backup (/usr/local/lib/gotham/freebsd10.1-amd64/prosave_backup.so)\n
> &nbsp;&nbsp;&nbsp;Access (/usr/local/lib/gotham/freebsd10.1-amd64/access.so)\n
> &nbsp;&nbsp;&nbsp;GDB (/usr/local/lib/gotham/freebsd10.1-amd64/gdb.so)\n
> &nbsp;&nbsp;&nbsp;Help (/usr/local/lib/gotham/freebsd10.1-amd64/help.so)\n
> &nbsp;&nbsp;&nbsp;Save (/usr/local/lib/gotham/freebsd10.1-amd64/save.so)\n
\n\n

### Load a module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>
> <span style="color:blue">(17:04:28) guillaume.friloux@botnet.master.com:</span> .module load gdb\n
> <span style="color:red">(17:04:29) botman:</span> Module gdb successfully loaded.
\n\n

### Unload a module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>
> <span style="color:blue">(17:02:34) guillaume.friloux@botnet.master.com:</span> .module unload gdb\n
> <span style="color:red">(17:02:34) botman:</span> Module gdb successfully unloaded.
