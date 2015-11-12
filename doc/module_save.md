# Save Module

<span class="label label-success">Alfred</span> <span class="label label-primary">Botman</span>

## Description

This module will backup any informations botman/alfred has, including but not limited to :
- Known contacts.
- `custom variables`.
- Loaded modules.

This module is more a demo than a real utility.\n
It might help you to export informations, or debug things, but that's all.\n
\n\n

## Configuration

Configuration is done inside `/usr/local/etc/gotham/modules.conf.d/save.conf` :
<pre>
{
   "interval": 30
}
</pre>

This interval represents the time between two export.\n
Exported file will be located inside `/usr/local/share/gotham/modules.d/save/` and will be in JSON.
\n\n

## Example use

### Get backup information

> <span style="color:blue">(15:18:18) guillaume.friloux@botnet.master.com:</span> .save\n
> <span style="color:red">(15:18:18) Alfred Pennyworth:</span> Dear guillaume.friloux@botnet.master.com, backup is made every 30 seconds. Last backup was on 2015/11/12 at 15:18:04, for a size of 446067 bytes. Sincerly yours, Alfred.

### Change interval of backups

> <span style="color:blue">(15:22:10) guillaume.friloux@botnet.master.com:</span> .save set interval 60\n
> <span style="color:red">(15:22:11) Alfred Pennyworth:</span> Modification done.
