<script id="tpl_stats" type="text/html">
<h1>System informations</h2>
   <ul>
      <li>Uptime : {{seconds_humanize uptime}}</li>
      <li>Memory used : {{size_humanize memory.used}}</li>
   </ul>

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

