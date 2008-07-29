{literal}
<script type="text/javascript">
    function toggle_tree(id, image_id, employee, depth) {
        var img = $(image_id).src;
        var extension = img.substr(img.lastIndexOf(".")+1, img.length);
        var filename = img.substr(0, img.lastIndexOf("."));
        if( 'plus' == filename.substr(filename.length-4) )
        {
          if( '' == $(id).innerHTML )
          {
            var nr_id = id.substr(id.lastIndexOf("_") +1 );
            var item_type = id.substr(0, id.lastIndexOf("_"));
            var start = $('startdate[year]').value+'-'+$('startdate[month]').value+'-'+$('startdate[day]').value;
            var end = $('enddate[year]').value+'-'+$('enddate[month]').value+'-'+$('enddate[day]').value;
            var projectId = $('project_id').value;

            new Ajax.Updater($(id), 'modules/project/update_resourceplan.php?'+ item_type +'=' + nr_id + '&action=unfold' + '&startdate=' + start + '&enddate=' + end + '&projectId=' + projectId + '&employeeId=' + employee + '&depth=' + (depth+1));
          }
          $(id).style.display = 'block';
          $(image_id).src = ( filename.substr(0, filename.length-4) + 'minus' + '.' + extension);
          $(image_id).alt = 'fold';
        }
        else
        {
          $(image_id).src = ( filename.substr(0, filename.length-5) + 'plus' + '.' + extension);
          $(image_id).alt = 'unfold';
          $(id).style.display = 'none';
        }
    }
</script>
{/literal}

{if $projectmenu != ''}
  {$projectmenu}<br/>
  <br/>
{/if}

<form name="resourceplaning" method="post" action="{atkdispatchfile}">
  {$session_form}

<div>
  {$timerange} <input type="submit" value="{atktext "goto"}">
</div>
<br /><br />

<div style="margin-bottom: 5px; background-color: #fff; position: relative; float: left; overflow: scroll; width: 1000px">

  <div style="margin-bottom: 5px; background-color: #ccc; position: relative; float: left; width: 100%; min-width: {$min_width}px;
    width: expression(document.body.clientWidth > ({$min_width}+30) ? '100%' : '{$min_width}px');">

    <div style="width: 190px; position: relative; float: left;">&nbsp;</div>
    {foreach from=$planheader item=header name=prop}
      <div style="width: 50px; position: relative; float: left;"><strong>{$header}</strong></div>
    {/foreach}

  </div>

    {foreach from=$resourceplan item=line}
    <div style="margin-bottom: 5px; background-color: #fff; position: relative; float: left; width: 100%; min-width: {$min_width}px;
      width: expression(document.body.clientWidth > ({$min_width}+30) ? '100%' : '{$min_width}px');">
      <div style="width: 190px; position: relative; float: left; ">
        <img src="images/plus.gif" id="img_fold_rp_{$line.id}" alt="unfold" onclick="toggle_tree('employee_{$line.id}', 'img_fold_rp_{$line.id}',{$line.employee},0); return false;" />
        {$line.name}
      </div>
        {foreach from=$line.data item=i name=prop}
          <div style="width: 50px; position: relative; float: left;">{$i}</div>
        {/foreach}
    </div>
    <div id="employee_{$line.id}" style="display:none;"></div>
  {/foreach}
</div>


  <input type="hidden" id="project_id" name="project_id" value="{$projectid}" >
</form>