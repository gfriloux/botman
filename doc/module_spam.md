# Spam Module

<span class="label label-success">Alfred</span>

## Description

This module ease the life of anyone trying to send messages to multiple botmans at once.\n
It accepts patterns to match botmans, based on `JID` or defined `custom variables`.
\n\n

## Configuration

You need to create a configuration file (`/usr/local/etc/gotham/modules.conf.d/spam.conf`) :
<pre>
{
   "vars": [
   ]
}
</pre>

Put any `custom variable` into the `vars` array and it will be matched
when sending spams.
\n\n

## Example use

> <span style="color:blue">(15:51:03) guillaume.friloux@botnet.master.com:</span> .spam svr2 .gdb list\n
> <span style="color:red">(15:51:03) Alfred Pennyworth:</span>\n
> Contacts that matched :\n
> &nbsp;&nbsp;&nbsp;online botman-x@botnet.master.com backup_licence[x] backup_server[svr2]\n
> &nbsp;&nbsp;&nbsp;online botman-y@botnet.master.com backup_licence[y] backup_server[svr2]\n
> \n
> <span style="color:red">(15:51:03) Alfred Pennyworth:</span>\n
> Answer from botman-x@botnet.master.com :\n
> \n
> List of coredumps :\n
> <table bordel=0 width=80%>
> <tr><td>&nbsp;</td><td>/var/dumps/gdb.60479.core</td><td>34308096</td><td>2015/10/21 10:39:45</td></tr>
> <tr><td></td><td>/var/dumps/bash.45189.core</td><td>18755584</td><td>2015/11/10 14:54:44</td></tr>
> </table>
> \n
> \n
> \n
> <span style="color:red">(15:51:03) Alfred Pennyworth:</span>\n
> Answer from botman-y@botnet.master.com :\n
> \n
> List of coredumps :\n
> <table bordel=0 width=80%>
> <tr><td>&nbsp;</td><td>/var/dumps/getty.735.core</td><td>14475264</td><td>2015/10/28 18:58:38</td></tr>
> <tr><td></td><td>/var/dumps/getty.707.core</td><td>22949888</td><td>2015/10/29 11:24:21</td></tr>
> <tr><td></td><td>/var/dumps/gdb.39656.core</td><td>18751488</td><td>2015/10/30 20:55:00</td></tr>
> <tr><td></td><td>/var/dumps/botman.39679.core</td><td>22949888</td><td>2015/10/30 20:55:00</td></tr>

So as you can see, when invoking the .spam command, the module will give you the list
of matching bots, and it will tell you the answer of each bot.\n
\n
This is a least 1 XMPP message for the list of matched bots, and 1 message per answer.\n
A timeout is set for bots not answering. It will tell you bots timing out on the command.
