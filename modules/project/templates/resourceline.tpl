{foreach from=$plan item=line}
  <div style="margin-bottom: 5px; background-color: #e7ebef; position: relative; float: left; width: 100%; min-width: {$min_width}px;
    width: expression(document.body.clientWidth > ({$min_width}+30) ? '100%' : '{$min_width}px');">
    <div style="width: {$width}px; position: relative; float: left;  padding-left: {$padding}px;">
      {if $line.type != 'task'}
        <img src="./images/plus.gif" id="img_{$line.employeeid}_{$line.type}_{$line.id}" alt="unfold" onclick="toggle_tree('{$line.type}_{$line.id}', 'img_{$line.employeeid}_{$line.type}_{$line.id}',{$line.employeeid},{$depth}); return false;" />
      {/if}
         <img src="./images/{$line.type}.gif" alt="{$line.type}" />
      {$line.name}
    </div>
      {foreach from=$line.data item=i name=prop}
        <div style="width: 50px; position: relative; float: left;">{$i}</div>
      {/foreach}
  </div>
  <div id="emp_{$line.employeeid}_{$line.type}_{$line.id}" style="display:none;"></div>
{/foreach}
