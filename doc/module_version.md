# Version Module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

## Description

This module will fetch version information of various packages (could also be
other stuff).\n
Informations are put under cache by alfred, allowing you to retrieve latest
recieved versions even if bot is offline.\n
\n\n

## Configuration

### Alfred

<pre>
{
   "vars": [
      "raid_status",
      "disk_full"
   ]
}
</pre>
The `vars` array contains the list of `custom variables` to be shown when
the `.version` command will list matching bots.

### Botman

<pre>
{
   "default_cmd" : "pkg info %s | grep 'Version' | cut -d':' -f2 | tr -d ' '",
   "versions_list" : [
      "botman",
      {
         "kernel": "uname -r"
      }
   ]
}
</pre>

- `default_cmd` : Default command to use to fetch version of packages.
- `versions_list` : Array of packages to list.\n If the array item is a string,
  then the `default_cmd` will be used to find version number.\n If the array
  item is an object, then it will use the given command.


\n\n

## Example use

### List versions of packages

<span class="label label-success">Alfred</span>
> <span style="color:blue">(24/11/2015 08:52:29) guillaume.friloux@botnet.master.com:</span> .version *xxx*\n
> <span style="color:red">(08:52:29) Alfred Pennyworth:</span>\n 
> Contacts that matched :\n
> &nbsp;&nbsp;&nbsp;online botman-xxx@botnet.master.com \n
> \n
> online botman-xxx@botnet.master.com  :\n
> &nbsp;&nbsp;&nbsp;kernel : 10.1-RELEASE\n
> &nbsp;&nbsp;&nbsp;botman : 1.2.13_5
\n\n

### Find bots having a particular version of one package

<span class="label label-success">Alfred</span>

This function allows to search over the version cache to find
bots matching given package version.\n
Supported conditions for package version matching are : <, <=, =, >=, >, !=

> <span style="color:blue">(08:59:47) guillaume.friloux@botnet.master.com:</span> .version find botman > 1.2.10\n
> <span style="color:red">(08:59:47) Alfred Pennyworth: </span>\n
> Contacts that matched “botman>1.2.10” :\n
> \n
> botman : 1.2.13_1\n
> &nbsp;&nbsp;&nbsp;online botman-aaa@botnet.master.com\n
> &nbsp;&nbsp;&nbsp;offline botman-bbb@botnet.master.com\n
> \n
> botman : 1.2.13_5\n
> &nbsp;&nbsp;&nbsp;online botman-ccc8@xmpp-test.asp64.lan\n
> &nbsp;&nbsp;&nbsp;online botman-xxx@xmpp-test.asp64.lan\n
> \n
> botman : 1.2.15_1\n
> &nbsp;&nbsp;&nbsp;offline botman-ddd@botnet.master.com\n
> &nbsp;&nbsp;&nbsp;online botman-eee@botnet.master.com\n
