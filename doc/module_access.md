# Access Module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

## Description

This module adds support for access rights over commands exported
by modules.\n
The access module uses a configuration file having 2 arrays :
- An array of commands, defining each command and the access level needed
  to use it.
- An array of users, having the username (patterns accepted), and its associated
  userlevel.

The higher the access level, the more commands you can use.\n
\n


## Configuration

For this module to work, you need a minimal json file used
as configuration file (`/etc/gotham/modules.conf.d/access.conf`)
<pre>
{
   "revision": 1,
   "citizens": [{
         "pattern":     "admin.*",
         "level":       10,
         "description": "Access rights for admins"
      }, {
         "pattern":     "support.*",
         "level":       5,
         "description": "Access rights for support guys"
   }],
   "commands": [{
         "pattern":     ".ssh *",
         "level":       5,
         "description": "Open or close a maintenance tunnel"
      }, {
         "pattern":     ".reboot *",
         "level":       10,
         "description": "Reboot system"
   }]
}
</pre>
The revision number is used to synchronize access rights over the botmans.\n
It will grow automatically as you set rights using `.access set` command.\n
\n

## Example use

### Get the list of access rights

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

This command will list every access entry set.\n
Only user access can be managed here. For commands level, it has to be done
manually in the configuration file.

> <span style="color:blue">(23/11/2015 15:00:12) guillaume.friloux@botman.master.com:</span> .access\n
> <span style="color:red">(15:00:12) Alfred Pennyworth:</span>\n
> List of access rights :\n
> &nbsp;&nbsp;&nbsp;0/ Pattern [botman.*] → Access level [1]\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Bots group\n
> &nbsp;&nbsp;&nbsp;1/ Pattern [admin.*] → Access level [10]\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Admins group\n
> &nbsp;&nbsp;&nbsp;2/ Pattern [dev.*] → Access level [8]\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Devs group\n
> &nbsp;&nbsp;&nbsp;3/ Pattern [support.*] → Access level [5]\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Supports group\n
\n

### Add a new access right

<span class="label label-success">Alfred</span>

This command allows you to add a pattern matching users to set access level.\n
The access right will be immediately applied to every bots.

> <span style="color:blue">(15:11:25) guillaume.friloux@botman.master.com:</span> .access add demo.* 1 Demonstration\n
> <span style="color:red">(15:11:25) Alfred Pennyworth:</span> Modification done\n
\n

### Change access level of a given entry

<span class="label label-success">Alfred</span>

This command will update an access entry to change the affected access level.

> <span style="color:blue">(15:13:05) guillaume.friloux@botman.master.com:</span> .access set demo.* 2\n
> <span style="color:red">(15:13:05) Alfred Pennyworth:</span> Modification done\n
\n

### Delete access entry

<span class="label label-success">Alfred</span>

> <span style="color:blue">(15:26:33) guillaume.friloux@botman.master.com:</span> .access del demo.*\n
> <span style="color:red">(15:26:33) Alfred Pennyworth:</span> Modification done\n
\n

### Synchronize access rights

<span class="label label-success">Alfred</span>

This command will sync current access rights to all the bots.\n
This might be usefull if one bot missed an update due to being offline by the
time the update happenned.

> <span style="color:blue">(15:36:28) guillaume.friloux@botman.master.com:</span> .access sync\n
> <span style="color:red">(15:36:28) Alfred Pennyworth:</span> Everyone seems to be up2date
