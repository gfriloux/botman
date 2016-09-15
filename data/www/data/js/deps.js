if (!String.prototype.trim) {
  String.prototype.trim = function () {
    return this.replace(/^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g, '');
  };
}

String.prototype.ltrim = function() {
   return this.replace(/^\s+/,"");
}

function humanFileSize(bytes, si) {
    var thresh = si ? 1000 : 1024;
    if(Math.abs(bytes) < thresh) {
        return bytes + ' B';
    }
    var units = si
        ? ['kB','MB','GB','TB','PB','EB','ZB','YB']
        : ['KiB','MiB','GiB','TiB','PiB','EiB','ZiB','YiB'];
    var u = -1;
    do {
        bytes /= thresh;
        ++u;
    } while(Math.abs(bytes) >= thresh && u < units.length - 1);
    return bytes.toFixed(1)+' '+units[u];
}

function secondsToString(seconds)
{
   var numyears = Math.floor(seconds / 31536000);
   var numdays = Math.floor((seconds % 31536000) / 86400); 
   var numhours = Math.floor(((seconds % 31536000) % 86400) / 3600);
   var numminutes = Math.floor((((seconds % 31536000) % 86400) % 3600) / 60);
   var numseconds = (((seconds % 31536000) % 86400) % 3600) % 60;
   var result = "";

   if (numyears > 0) result += numyears + " years ";
   if (numdays > 0) result += numdays + " days ";
   if (numhours > 0) result += numhours + " hours ";
   if (numminutes > 0) result += numminutes + " minutes ";
   result += numseconds + " seconds";

   return result;
}

Handlebars.registerHelper('if_eq', function(a, b, opts)
{
   if (a == b)
     return opts.fn(this);
   return opts.inverse(this);
});

Handlebars.registerHelper('jid_username', function(jid) {
   var pos = jid.indexOf("@");
   var str = jid.substr(0, pos);
   return str;
});
