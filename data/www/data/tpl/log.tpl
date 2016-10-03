<script id="tpl_log_entries" type="text/html">
   {{#results}}
      <div class="panel panel-primary">
         <div class="panel-heading">
            {{id}} - {{jid_username source}} <ul class="navbar-right">{{date}}&nbsp;&nbsp;</ul>
         </div>
         <div class="panel-body">
            {{#if_eq type 0}}
               Client Connected
            {{/if_eq}}
            {{#if_eq type 1}}
               Client Disconnected
            {{/if_eq}}
            {{#if_eq type 2}}
               <pre>{{{data}}}</pre>
            {{/if_eq}}
         </div>
      </div>
   {{/results}}
</script>

<script id="tpl_log_filters" type="text/html">
   <div class="list-group">
   {{#.}}
      <a href="#" class="list-group-item">{{this}}</a>
   {{/.}}
   </div>
</script>
