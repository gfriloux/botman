<script id="tpl_stats" type="text/html">
<h1>System informations</h2>

   <div class="row">
   <div class="col-lg-4">
      <div class="well text-center">
         <h3>{{seconds_humanize uptime}}</h3>
         Uptime
      </div>
   </div>
   <div class="col-lg-3">
      <div class="well text-center">
         <h3>{{size_humanize memory.used}}</h3>
         Memory
      </div>
   </div>
   </div>

<h1>Last 20 queries</h1>

<table class="table table-striped table-hover ">
<thead>
   <tr>
      <th>Time</th>
      <th>Source</th>
      <th>Command</th>
   </tr>
</thead>
<tbody>
{{#queries}}
   <tr>
      <td>{{date_humanize timestamp}}</td>
      <td>{{source}}</td>
      <td>{{command}}</td>
   </tr>
{{/queries}}
</tbody>
</table>

<h1>Contacts</h1>

{{#contacts}}
<div class="col-lg-4">
{{#online}}
<a href="#" class="btn btn-block btn-success">
{{/online}}
{{^online}}
<a href="#" class="btn btn-block btn-danger">
{{/online}}
   <img src="data/img/{{type}}.png" width="40">&nbsp;{{jid}}
</a>
</div>
{{/contacts}}

</script>

