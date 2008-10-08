<table border="0" cellpadding="0" cellspacing="0" class="tabsTabs mainTabs">
  <tr>         
    <td id="tab_1" valign="middle" align="left" nowrap="nowrap" class="{if $activetab == 'add'}activetab{else}passivetab{/if}">  
      <a href="{$taburl.add}" title="{atktext id="timereg_advanced"}">{atktext id="timereg_advanced"}</a>
    </td>          
    <td>&nbsp;</td>
    <td id="tab_1" valign="middle" align="left" nowrap="nowrap" class="{if $activetab == 'favorites'}activetab{else}passivetab{/if}">  
      <a href="{$taburl.favorites}" title="{atktext id="timereg_favorites"}">{atktext id="timereg_favorites"}</a>
    </td>          
    <td>&nbsp;</td>
    <td id="tab_1" valign="middle" align="left" nowrap="nowrap" class="{if $activetab == 'times'}activetab{else}passivetab{/if}">  
      <a href="{$taburl.times}" title="{atktext id="timereg_hours"}">{atktext id="timereg_hours"}</a>
    </td>          
  </tr>
</table>


{if $rows|count == 0}

{atktext id="no_hours_registered"}

{else}


<table cellspacing="0" cellpadding="0" class="recordListContainer">
  <tr>
    <td>
    
      <table class="recordList" cellpadding="0" cellspacing="0">    
          
        <!-- header -->
        <tr>
          <th class="recordListThFirst"><b>{atktext id="project"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="phase"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="enddate"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="time"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="completed"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="comment"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="activity"}</b></th>
        </tr>

        <!-- records -->

        {foreach from=$rows item=row}
         <tr id="r1_{$row.id}" class="row{if $row.rownum % 2 == 0 }1{else}2{/if}" onmouseover="highlightrow(this, '#eeeeee')" onmouseout="resetrow(this)">
           <td class="recordListTd">
             <a href="{$project_url}&atkselector=project.id%3D%27{$row.phaseid.projectid.id}%27">{$row.projectid.name}</a>
           </td>
           <td class="recordListTd">
             <a href="{$phase_url}&atkselector=phase.id%3D%27{$row.phaseid.id}%27&atkfilter=projectid%3D{$row.phaseid.projectid.id}">{$row.phaseid.name}</a>
           </td>
           <td class="recordListTd">
             {$row.activitydate.day}-{$row.activitydate.month}-{$row.activitydate.year}
           </td>
           <td class="recordListTd">
             {$row.time_full}
           </td>
           <td class="recordListTd">
             {$row.phaseid.completed} %
           </td>
           <td class="recordListTd">
             {$row.remark}
           </td>
           <td class="recordListTd">
             {$row.activityid.name}
           </td>
        </tr>
        {/foreach}

      </table>
      
    </td>
  </tr>
  
</table>

{/if}
