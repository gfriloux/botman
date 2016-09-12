<script id="tpl_query_bot_filter" type="text/html">
  <div class="btn-group">
    <a href="#" class="btn btn-danger">Filter : {{filter}}</a>
    <a href="#" class="btn btn-danger dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></a>
    <ul class="dropdown-menu">
      <li><a href="#">Set</a></li>
      <li><a href="#">Refresh</a></li>
      <li class="divider"></li>
      <li><a href="#">Reset</a></li>
    </ul>
  </div>
</script>

<script id="tpl_query_bot_list" type="text/html">
<div class="list-group">
  {{#.}}
  <a href="#" class="list-group-item">{{jid}}</a>
  {{/.}}
</div>
</script>

<script id="tpl_query_history" type="text/html">
  <div class="panel-body">
{{#.}}
    <h4>{{jid}}</h4>
   <pre>{{message}}</pre>
{{/.}}
  </div>
</script>

<script id="tpl_query_title" type="text/html">
  <div class="panel-heading">
    <h4 class="panel-title">{{title}}</h4>
  </div>
</script>
