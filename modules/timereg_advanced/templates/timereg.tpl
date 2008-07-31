<table cellspacing="0" cellpadding="0" class="recordListContainer">
  <tr>
    <td>
    {if count($errors)}
      <div style="color: red;">
        <strong>{atktext id="error_formdataerror"}</strong><br/><br/>
		    {section name=i loop=$errors}
		    <u>{$errors[i].name}:</u><br/>
		    {foreach from=$errors[i].err item=err}
		      {$err}<br/>
		    {/foreach}
		    <br/>
        {/section}
		  </div>
		{/if}
    
      {$formstart}
    
      <table class="recordList" cellpadding="0" cellspacing="0">    
          
        <!-- header -->
        <tr>
          <th class="recordListThFirst">&nbsp;</th>
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
           <td class="recordListTdFirst">
             <input type="checkbox" id="cb_{$row.id}" name="timereg[{$row.id}][id]"{if $values.timereg[$row.id].id == "on"} checked="checked"{/if} />
           </td>
           <td class="recordListTd">
             {$row.projectid.name}
           </td>
           <td class="recordListTd">
             {$row.name}
             <input type="hidden" name="timereg[{$row.id}][name]" value="{$row.name}" />               
             <input type="hidden" name="timereg[{$row.id}][phaseid]" value="{$row.id}" />
           </td>
           <td class="recordListTd">
             {$row.enddate.day}-{$row.enddate.month}-{$row.enddate.year}
           </td>
           <td class="recordListTd">
             <select id="timereg[{$row.id}][time][hours]" name="timereg[{$row.id}][time][hours]" onchange="Timereg_Advanced.updateCheckbox({$row.id})">
             {section name=h loop=$times.hours}
               <option value="{$times.hours[h]}"{if $values.timereg[$row.id].time.hours == $times.hours[h]} selected="selected"{/if}>{$times.hours[h]} {atktext id="hours"}</option>
             {/section}
             </select> : 
             <select id="timereg[{$row.id}][time][minutes]" name="timereg[{$row.id}][time][minutes]" onchange="Timereg_Advanced.updateCheckbox({$row.id})">
             {section name=m loop=$times.minutes}
               <option value="{$times.minutes[m]}"{if $values.timereg[$row.id].time.minutes == $times.minutes[m]} selected="selected"{/if}>{$times.minutes[m]} {atktext id="minutes"}</option>
             {/section}
             </select>
           </td>
           <td class="recordListTd">
             <select id="timereg[{$row.id}][completed]" name="timereg[{$row.id}][completed]" onchange="Timereg_Advanced.updateCheckbox({$row.id})">
              {section name=c loop=$completed}
               {if '' != $values.timereg[$row.id].completed}
                 {assign var="progress" value=$values.timereg[$row.id].completed}
               {else}
                 {assign var="progress" value=$row.completed}
               {/if}
               <option value="{$completed[c]}"{if $completed[c] == $progress} selected="selected"{/if}>{$completed[c]}%</option>
              {/section}
             </select>
           </td>
           <td class="recordListTd">
             <input type="text" size="30" id="timereg[{$row.id}][remark]" name="timereg[{$row.id}][remark]" value="{$values.timereg[$row.id].remark}" onchange="Timereg_Advanced.updateCheckbox({$row.id})" />
           </td>
           <td class="recordListTd">
             <select id="timereg[{$row.id}][activity]" name="timereg[{$row.id}][activity]" onchange="Timereg_Advanced.updateCheckbox({$row.id})">
             {foreach from=$row.activities item=activity}
               <option value="{$activity.activityid.id}"{if $values.timereg[$row.id].activity == $activity.activityid.id} selected="selected"{/if}>{$activity.activityid.name}</option>
             {/foreach}
             </select>
           </td>
        </tr>
        {/foreach}

      </table>
      
      <br/><br/>
      
			<div id="action-buttons">
	      {foreach from=$buttons item=button}
	        &nbsp;{$button}&nbsp;
	      {/foreach}
			</div>
      
      {$formend}
    </td>
  </tr>
  
</table>

