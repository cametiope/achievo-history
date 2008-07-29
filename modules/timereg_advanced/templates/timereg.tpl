<table cellspacing="0" cellpadding="0" class="recordListContainer">
  <tr>
    <td>
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
                <input type="checkbox" name="timereg[{$row.id}][id]" />
              </td>
              <td class="recordListTd">
                {$row.projectid.name}
              </td>
              <td class="recordListTd">
                {$row.name}
              </td>
              <td class="recordListTd">
                {$row.enddate.day}-{$row.enddate.month}-{$row.enddate.year}
              </td>
              <td class="recordListTd">
                <input type="text" size="4" maxlength="2" name="timereg[{$row.id}][hours]" /> : <input type="text" size="4" maxlength="2" name="timereg[{$row.id}][minuted]" />
              </td>
              <td class="recordListTd">
                <select name="timereg[{$row.id}][completed]">
                  <option value="0">0%</option>
                </select>
              </td>
              <td class="recordListTd">
                <input type="text" size="30" name="timereg[{$row.id}][description]" />
              </td>
              <td class="recordListTd">
                <select name="timereg[{$row.id}][activity]">
                  <option value="x">Actvity X</option>
                  <option value="y">Actvity Y</option>
                  <option value="z">Actvity Z</option>
                </select>
              </td>
          </tr>
          {/foreach}

      </table>
    </td>
  </tr>
  
</table>

<input type="submit" name="submit" value="{atktext id="submit"}" />