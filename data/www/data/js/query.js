var commands = [
   { "type" : "title"  , "name" : "System informations"             , "command" : ""                              },
   { "type" : "command", "name" : "Get uptime"                      , "command" : ".uptime"                       },
   { "type" : "command", "name" : "List processes"                  , "command" : ".psax"                         },
   { "type" : "command", "name" : "Get software versions"           , "command" : ".version"                      },
   { "type" : "divider", "name" : ""                                , "command" : ""                              },
   { "type" : "title"  , "name" : "Debugging commands"              , "command" : ""                              },
   { "type" : "command", "name" : "List coredumps"                  , "command" : ".gdb list"                     },
   { "type" : "command", "name" : "Retrieve Backtrace from coredump", "command" : ".gdb fetch coredump_filename"  },
   { "type" : "command", "name" : "Delete coredump"                 , "command" : ".gdb delete coredump_filename" },
   { "type" : "divider", "name" : ""                                , "command" : ""                              },
   { "type" : "title"  , "name" : "Maintenance commands"            , "command" : ""                              },
   { "type" : "command", "name" : "Show SSH tunnel informations"    , "command" : ".ssh"                          },
   { "type" : "command", "name" : "Open SSH tunnel"                 , "command" : ".ssh on"                       },
   { "type" : "command", "name" : "Close SSH tunnel"                , "command" : ".ssh off"                      },
   { "type" : "divider", "name" : ""                                , "command" : ""                              },
   { "type" : "command", "name" : "Reboot"                          , "command" : ".reboot"                       },
];
var actual_filter;

function commands_helper()
{
   for (var i = 0; i < commands.length; i++)
   {
      switch (commands[i].type)
      {
         case "title":
            $('#commands').append('<li>'+commands[i].name+'</li>');
            break;
         case "divider":
            $('#commands').append('<li class="divider"></li>');
            break;
         case "command":
            var html;

            html = "<li><a href=# id=command_"+i+">"+commands[i].name+"</a></li>";
            $('#commands').append(html);

            $("#command_"+i).click(function() {
               var i = $(this).attr('id').substr(8,10);
               $("#query_command").val(commands[i].command);
            });
            break;
         default:
            break;
      }
   }
}

function register_events()
{
   commands_helper();

   $("#query_send").click(function() {
      var str = $("#query_command").val();
      if (!str || str === "") return;
      query_send(str);
      $("#query_command").val("");
   });

   $('#query_command').keyup(function(e){
      if (e.keyCode != 13) return;
      query_send($("#query_command").val());
      $("#query_command").val("");
   });

   $("#query_clear").click(function() {
      $("#query_history").html('');
   });
}

function seen_query(filter)
{
   var uri = "seen/"+filter;
   console.log("Querying seen uri "+uri);
   $.getJSON(uri, function(data, textStatus, jqXHr) {
      $.get('data/tpl/query.tpl', function(template, textStatus, jqXhr) {
         var source = $(template).filter('#tpl_query_bot_list').html();
         var template = Handlebars.compile(source);
         $('#query_bot_list').html(template(data));
      });
   })
   .done(function() {})
   .fail(function(jqxhr, textStatus, error) {
      var err = textStatus + ', ' + error;
      console.log( "Request Failed: " + err);
   });
}

function filter_input_show()
{
   var len;

   $('#filter_edit').show();
   $('#filter_input').show();
   $('#filter_button').css('z-index', -10);
   $('#filter_input').css('z-index', 10);
   $('#filter_input').focus();

   len = $('#filter_input').length * 2;
   $('#filter_input')[0].setSelectionRange(len, len);
}

function filter_input_hide()
{
   $('#filter_edit').hide();
   $('#filter_button').show();
   $('#filter_button').css('z-index', 10);
   $('#filter_input').css('z-index', -10);
}

function filter_update(filter_attr)
{
   var json = { filter : filter_attr };
   console.log("Updating filter to : "+filter_attr);
   $.get('data/tpl/query.tpl', function(template, textStatus, jqXhr) {
      var source = $(template).filter('#tpl_query_bot_filter').html();
      var template = Handlebars.compile(source);
      $('#query_bot_filter').html(template(json));

      $('#filter_edit').hide();

      $('#filter_button').on('click', function (event) {
         filter_input_show();
      });

      $('#filter_input').focusout(function() {
         filter_input_hide();
      });

      $('#filter_edit').keyup(function(e){
         if (e.keyCode != 13) return;

         filter_input_hide();
         seen_query($("#filter_input").val());
         filter_update($("#filter_input").val());
      });

      $("#query_filter_refresh").click(function() {
         console.log("Refresh click!");
         seen_query(filter_attr);                  
      });

      $("#query_filter_reset").click(function() {
         console.log("Reset click!");
         filter_update("*");
         seen_query("*");                  
      });
   });
   actual_filter = filter_attr;
}

function query_send_default(data, command, tpl_filter)
{
   var json = { title : command };
   var text_rendered;
   var source

   $.get('data/tpl/query.tpl', function(template, textStatus, jqXhr) {
      source = $(template).filter(tpl_filter).html();
      var template = Handlebars.compile(source);

      $.get('data/tpl/query.tpl', function(template2) {
         source = $(template2).filter('#tpl_query_title').html();
         var template2 = Handlebars.compile(source);

         text_rendered  = "<div class=\"panel panel-primary\">";
         text_rendered += template2(json);
         text_rendered += template(data);
         text_rendered += "</div>";
         $('#query_history').append(text_rendered);
         $('#query_history').animate({ scrollTop: $('#query_history')[0].scrollHeight+9999 }, "slow");
      });
   });
}

function query_send(command)
{
   var uri = "spam/"+actual_filter;

   console.log("Querying spam uri "+uri);
   $.post(uri, command)
   .done(function(data) {
      if (command.startsWith(".uptime") == true)
        query_send_default(data, command, '#tpl_query_uptime');
      else query_send_default(data, command, '#tpl_query_history');
   })
   .fail(function(jqxhr, textStatus, error) {
      var err = textStatus + ', ' + error;
      console.log( "Request Failed: " + err);
   });
}

function parse_uptime(uptime)
{
  var tmp;
  var len_up;
  var len_days;
  var len_hours;
  var len_users;
  var len_load;
  var uptime_json = {
    time  : "",
    days  : "",
    hours : "",
    users : "",
    load  : ""
  };

  len_up = uptime.indexOf("up");
  uptime_json.time = uptime.substr(0, len_up);
  uptime_json.time = uptime_json.time.trim();
  
  len_days = uptime.indexOf(",");
  tmp = uptime.substr(len_up+2, len_days - len_up-2);
  if (tmp.includes("day") == true)
    {
       uptime_json.days = tmp;
       len_hours = uptime.indexOf(",", len_days + 1);
       uptime_json.hours = uptime.substr(len_days + 1, len_hours - len_days - 1);
    }
  else
    {
       uptime_json.hours = tmp;
       len_hours = len_days;
    }
  uptime_json.days = uptime_json.days.trim();
  uptime_json.hours = uptime_json.hours.trim();
  
  len_users = uptime.indexOf(",", len_hours + 1);
  uptime_json.users = uptime.substr(len_hours + 1, len_users - len_hours - 1);
  uptime_json.users = uptime_json.users.trim();
  
  len_load = uptime.indexOf(":", len_users + 1);
  uptime_json.load = uptime.substr(len_load + 1);
  uptime_json.load = uptime_json.load.trim();
  
  //console.log(JSON.stringify(uptime_json, null, 4));
  return uptime_json;
}

Handlebars.registerHelper('query_uptime', function(message) {
   var json = parse_uptime(message);
   var text;

   text  = '<span class="glyphicon glyphicon-time" aria-hidden="true"></span> ' + json.time + '<br />';
   text += '<span class="glyphicon glyphicon-refresh" aria-hidden="true"></span> ' + json.days + ' ' + json.hours + '<br />';
   text += '<span class="glyphicon glyphicon-user" aria-hidden="true"></span> ' + json.users + '<br />';
   text += '<span class="glyphicon glyphicon-tasks" aria-hidden="true"></span> ' + json.load + '<br />';
   return text;
});
