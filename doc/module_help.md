# Help Module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

## Description

This module will display a list of loaded modules, and for each of them,
the registered commands and their description.\n
If the `Access` module is loaded, it will also print the access level needed to use
the command (command is hidden if your level is too low).\n

Nothing has to be done for a module to be detected by the `Help` module.\n
Your module only need to register its commands to the `Gotham` library for the `Help`
module to see it.
\n\n

## Configuration

This module does not need any configuration to be done.
\n\n

## Example use

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>
> <span style="color:blue">(16:24:13) guillaume.friloux@botnet.master.com:</span> .help\n
> <span style="color:red">(16:24:13) botman-bull - asp6400037: </span>\n
> List of commands :\n
> &nbsp;&nbsp;&nbsp;Commands for module Version :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.version (1) : [.version] - This command returns the version of some installed softwares. Software list and custom version commands can be specified in version.conf.\n
> \n
> &nbsp;&nbsp;&nbsp;Commands for module Install :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.install (1) : [.install softwarelist] - Install softwarelist.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.upgrade (1) : [.upgrade] - Performs an upgrade on all repositories.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.jobs (1) : [.jobs] - List running install / upgrade jobs\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.kill (1) : [.jobs job_id] - Kill running install / upgrade job.\n
> \n
> &nbsp;&nbsp;&nbsp;Commands for module Module :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.module (1) : [.module] - This command allows you to list loaded modules.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.module load (1) : [.module load module_name] - This command allows you to load a module.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.module unload (1) : [.module unload module_name] - This command allows you to unload a module.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.module reload (1) : [.module reload module_name] - This command allows you to reload a module.\n
> \n
> &nbsp;&nbsp;&nbsp;Commands for module Ssh_Tunnel :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.ssh (1) : [.ssh] - This command will show informations about the tunnel (opened or not, which port)\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.ssh on (1) : [.ssh on] - This command will try to open a tunnel and send back the port to use.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.ssh off (1) : [.ssh off] - This command will shut down the tunnel if it's opened.\n
> \n
> &nbsp;&nbsp;&nbsp;Commands for module Sysinfo :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.sysinfo (1) : [.sysinfo] - This command allows you to get some system informations (manufacturer, serial #, ...)\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.psax (1) : [.psax] - Run ps ax on server\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.reboot (1) : [.reboot] - Run reboot on server\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.uptime (1) : [.uptime] - Run uptime on server\n
> \n
> &nbsp;&nbsp;&nbsp;Commands for module Access :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.access (10) : This command will list all user accesses.\n
> \n
> &nbsp;&nbsp;&nbsp;Commands for module GDB :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.gdb list (1) : [.gdb list] - This command will list coredumps inside the coredumps directory.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.gdb delete (1) : [.gdb delete <pattern>] - This command will delete coredumps matching given parttern.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.gdb fetch (1) : [.gdb fetch <coredump>] - This command will retrieve the backtrace from a given coredump.\n
> \n
> &nbsp;&nbsp;&nbsp;Commands for module Help :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.help (1) : [.help] - This command allows you to list all the commands registered by modules.\n
> \n
> &nbsp;&nbsp;&nbsp;Commands for module Save :\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.save (1) : [.save] - This command will show informations about the backup.\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.save set (1) : [.save set variable value] - This command allows you to set a variable. Possible variables : 'interval'.\n

