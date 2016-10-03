var actual_limit = 10;
var actual_page = 1;
var actual_filters = [];

function log_last_fetch(filters, limit, page)
{
   var uri = "log/last/"+limit+"/"+page;

   console.log("Fetching logs uri "+uri);
   $.post(uri, JSON.stringify(actual_filters))
   .done(function(data) {
      var total_pages;

      total_pages  = data.total_results / data.limit;
      actual_limit = data.limit;
      actual_page  = data.page;
      actual_filters = data.filters;

      $('#log_page').html(data.page);
      $('#log_page_total').html("/ "+Math.ceil(total_pages));

      $.get('data/tpl/log.tpl', function(template) {
         var source = $(template).filter('#tpl_log_entries').html();
         var template = Handlebars.compile(source);
         $('#query_history').html(template(data));
      });
   })
   .fail(function(jqxhr, textStatus, error) {
      var err = textStatus + ', ' + error;
      console.log( "Request Failed: " + err);
   });
}

function filters_draw(array_str)
{
   $.get('data/tpl/log.tpl', function(template) {
      var source = $(template).filter('#tpl_log_filters').html();
      var template = Handlebars.compile(source);
      var render = template(array_str)
      $('#filter_list').html(render);
   });
}
