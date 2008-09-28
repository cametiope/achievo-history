{literal}
<script type="text/javascript">
    function toggle_tree(id, image_id, employeeId, depth) {
        var nr_id = id.substr(id.lastIndexOf("_") +1 );
        var item_type = id.substr(0, id.lastIndexOf("_"));
        id = 'emp_'+employeeId+'_'+id;
        var img = $(image_id).src;
        var extension = img.substr(img.lastIndexOf(".")+1, img.length);
        var filename = img.substr(0, img.lastIndexOf("."));
        if( 'plus' == filename.substr(filename.length-4) )
        {
          if( '' == $(id).innerHTML )
          {
            var start = $('startdate[year]').value+'-'+$('startdate[month]').value+'-'+$('startdate[day]').value;
            var end = $('enddate[year]').value+'-'+$('enddate[month]').value+'-'+$('enddate[day]').value;
            var viewmode = $('view').value;

            new Ajax.Updater($(id), 'modules/project/update_resourceplan.php?'+ 'type='+ item_type +'&id=' + nr_id + '&action=unfold' + '&startdate=' + start + '&enddate=' + end + '&view=' + viewmode + '&employeeId=' + employeeId + '&depth=' + (depth+1));
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
  <div>{$viewmode}</div>
  <br /><br />
  
  <div style="margin-bottom: 5px; background-color: #fff; position: relative; float: left; overflow: scroll; width: 1000px">
  
    <div style="margin-bottom: 5px; background-color: #ccc; position: relative; float: left; width: 100%; min-width: {$min_width}px;
      width: expression(document.body.clientWidth > ({$min_width}+30) ? '100%' : '{$min_width}px');">
  
      <div style="width: 190px; position: relative; float: left;">&nbsp;</div>
        {foreach from=$planheader item=header name=prop}
          <div style="width: 50px; position: relative; float: left;"><strong>{$header}</strong></div>
        {/foreach}
  
      </div>
  
      {atkmoduledir modulename="project"}
      {include file="file:$atkmoduledir/templates/resourceline.tpl"}
    
  </div>
  <input type="hidden" id="view" name="view" value="{$view}" >
</form>