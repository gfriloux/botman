# Install Module

<span class="label label-primary">Botman</span>

## Description

This module Is in charge of running installation of package or system upgrades.\n

## Configuration

You need to write a configuration file for this module.\n
Create the file `/etc/gotham/modules.conf.d/install.conf` that might look as follows :
<pre>
{
   "install": {
      "pre": "",
      "cmd": "yes | pkg install %s",
      "post": ""
   },
   "upgrade": {
      "pre": "freebsd-update cron",
      "cmd": "freebsd-update install",
      "post": ""
   },
   "vars": [
   ]
}
</pre>
You have to specify the commands you need to execute for installation of upgrade to be effective.\n
This allows the module to adapt itself to various distros.
\n\n

## Example use

### Install a package

> <span style="color:blue">(31/07/2015 08:45:26) guillaume.friloux@botnet.master.com:</span> .install htop\n
> <span style="color:red">(08:45:26) botman-xxxx:</span> {\n
> &nbsp;&nbsp;&nbsp;"command":  ".install_begins",\n
> &nbsp;&nbsp;&nbsp;"parameters":  "htop",\n
> &nbsp;&nbsp;&nbsp;"status":   "ok",\n
> &nbsp;&nbsp;&nbsp;"content":  ["Software(s) install/upgrade will begin in a few seconds."]\n
> }\n
> <span style="color:red">(08:45:45) botman-xxxx:</span> {\n
> &nbsp;&nbsp;&nbsp;"command":  ".install",\n
> &nbsp;&nbsp;&nbsp;"parameters":  "htop",\n
> &nbsp;&nbsp;&nbsp;"status":   "ok",\n
> &nbsp;&nbsp;&nbsp;"content":  [\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Updating private repository catalogue...",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Fetching meta.txz: . done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Fetching packagesite.txz: .. done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Processing entries: ..... done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"amtrust repository update completed. 40 packages processed.",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Updating FreeBSD repository catalogue...",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Fetching meta.txz: . done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Fetching packagesite.txz: .......... done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Processing entries: .......... done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"FreeBSD repository update completed. 24389 packages processed.",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Updating database digests format: .. done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"The following 2 package(s) will be affected (of 0 checked):",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"New packages to be INSTALLED:",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"\thtop: 1.0.3 [FreeBSD]",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"\tlsof: 4.89.e,8 [FreeBSD]",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"The process will require 377 KiB more space.",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"170 KiB to be downloaded.",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Proceed with this action? [y/N]: Fetching htop-1.0.3.txz: ....... done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Fetching lsof-4.89.e,8.txz: .......... done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Checking integrity... done (0 conflicting)",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"[1/2] Installing lsof-4.89.e,8...",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"[1/2] Extracting lsof-4.89.e,8: .......... done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"[2/2] Installing htop-1.0.3...",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"[2/2] Extracting htop-1.0.3: ....... done",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"Message for htop-1.0.3:",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"***********************************************************",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"htop(1) requires linprocfs(5) to be mounted. If you don't",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"have it mounted already, please add this line to /etc/fstab",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"and run `mkdir -p /usr/compat/linux/proc; ln -s /usr/compat /compat; mount linproc`:",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"linproc /compat/linux/proc linprocfs rw,late 0 0",\n
> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"***********************************************************"\n
> &nbsp;&nbsp;&nbsp;]\n
> }\n
