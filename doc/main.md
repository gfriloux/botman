Botman {#mainpage}
======

Botman is a project that aims to ease management of servers throught the use
of the XMPP messaging system.\n
We chose to use XMPP to avoid various problemes as having delays in sending commands,
or the use of NATed networks.\n\n

This project assembles one xmpp bot library, modules, and two binaries.
\n\n

## Botman

Botman is a bot to install on each machine you want to command.\n
Working around modules, its capabilities can be expanded quite easilly.\n

## Alfred

Alfred is an assistant to help you to manager all your bots.\n
His main role is to keep datas in cache, helping launching mass commands,
and make it easier to manager them in general.\n\n

As of Botman, Alfred works around modules. Its capacities can then be expanded
quite easilly.\n\n

Most of the time, you will prefer talking to Alfred instead of botmans directly.\n
It saves you the time of adding each botman in your contact list and allows fast searchs on
various criteria to find the corresponding botman.

## Modules

Each module is designed to provide one functionnality.\n
Additionnal modules can be written.\n
Modules can talk to each others if needed.\n
\n

List of modules that comes by default :
- [access](md_module_access.html) **TODO**
- [gdb](md_module_gdb.html)
- [help](md_module_help.html)
- [install](md_module_install.html)
- [info](md_module_info.html)
- [module](md_module_module.html) **TODO**
- [rewrite](md_module_rewrite.html)
- [save](md_module_save.html)
- [seen](md_module_seen.html)
- [spam](md_module_spam.html)
- [ssh_tunnel](md_module_ssh_tunnel.html) **TODO**
- [sysinfo](md_module_sysinfo.html) **TODO**
- [version](md_module_version.html) **TODO**
