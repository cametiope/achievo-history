{literal}
<script type="text/javascript">
    function toggle_tree(id, image_id) {
        var img = $(image_id).src;       
        var extension = img.substr(img.lastIndexOf(".")+1, img.length);
        var filename = img.substr(0, img.lastIndexOf("."));
        if( 'plus' == filename.substr(filename.length-4) )
        {
          if( '' == $(id).innerHTML )
          {
            var nr_id = id.substr(id.lastIndexOf("_") +1 );
            var item_type = id.substr(0, id.lastIndexOf("_"));
            new Ajax.Updater($(id), 'modules/project/update_milestone_tree.php?'+ item_type +'=' + nr_id + '&action=unfold');        
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

<div>
  {$addlink}
</div>
<br /><br />

<div style="margin-bottom: 5px; background-color: #ccc; position: relative; float: left; width: 100%;">
  <div style="width: 350px; position: relative; float: left"><strong>{$header_titles[0]}</strong></div>
  <div style="width: 120px; position: relative; float: left"><strong>{$header_titles[1]}</strong></div>
  <div style="width: 120px; position: relative; float: left"><strong>{$header_titles[2]}</strong></div>
  <div style="width: 120px; position: relative; float: left"><strong>{$header_titles[3]}</strong></div>
  <div style="width: 120px; position: relative; float: left"><strong>{$header_titles[4]}</strong></div>
  <div style="width: 60px; position: relative; float: left"><strong>{$header_titles[5]}</strong></div>
</div>


{section name=i loop=$milestones}
<div style="margin-bottom: 5px; background-color: #fff; position: relative; float: left; width: 100%;">
  <div style="width: 350px; position: relative; float: left">
    <img src="images/plus.gif" id="img_fold_ms_{$milestones[i].id}" alt="unfold" onclick="toggle_tree('milestone_{$milestones[i].id}', 'img_fold_ms_{$milestones[i].id}'); return false;" />
    {$milestones[i].name}
  </div>      
  <div style="width: 120px; position: relative; float: left">{$milestones[i].duedate.day}.{$milestones[i].duedate.month}.{$milestones[i].duedate.year}</div>
  <div style="width: 120px; position: relative; float: left">{$milestones[i].progress}</div>
  <div style="width: 120px; position: relative; float: left">{$milestones[i].userid.lastname}</div>
  <div style="width: 120px; position: relative; float: left">{$milestones[i].status}</div>
  <div style="width: 60px; position: relative; float: left">{$milestones[i].editlink} {$milestones[i].deletelink}</div>
</div>
<div id="milestone_{$milestones[i].id}" style="display:none;"></div>
{/section}


<div style="clear: both"></div>