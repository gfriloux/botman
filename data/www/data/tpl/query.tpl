<script id="tpl_query_bot_filter" type="text/html">
  <div class="btn-group" id="filter">
    <a href="#" class="btn btn-danger" id="filter_button">Filter : {{filter}}</a>
    <div id="filter_edit">
       <input type="text" value="{{filter}}" id="filter_input"/>
    </div>
    <a href="#" class="btn btn-danger dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></a>
    <ul class="dropdown-menu">
      <li><a href="#" id="query_filter_refresh">Refresh</a></li>
      <li class="divider"></li>
      <li><a href="#" id="query_filter_reset">Reset</a></li>
    </ul>
  </div>
</script>

<script id="tpl_query_bot_list" type="text/html">
<div class="list-group">
  {{#.}}
  {{#if_eq state "online"}}
  <a href="#" class="list-group-item"><span class="text-success">{{jid_username jid}}</span></a>
  {{else}}
  <a href="#" class="list-group-item"><span class="text-danger">{{jid_username jid}}</span></a>
  {{/if_eq}}
  {{/.}}
</div>
</script>

<script id="tpl_query_history" type="text/html">
  <div class="panel-body">
{{#.}}
    <h4>{{jid_username jid}}</h4>
   <pre>{{message}}</pre>
{{/.}}
  </div>
</script>

<script id="tpl_query_title" type="text/html">
  <div class="panel-heading">
    <h4 class="panel-title"><b>{{title}}</b></h4>
  </div>
</script>
