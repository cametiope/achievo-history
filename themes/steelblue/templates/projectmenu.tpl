<table border="0" cellpadding="0" cellspacing="0" class="tabsTabs mainTabs">
  <tr>         
    {section name=i loop=$tabs}                     
    <td id="tab_{$smarty.section.i.index}" valign="middle" align="left" nowrap="nowrap" class="{if $tabs[i].active == 1}activetab{else}passivetab{/if}">  
      <a href="{$tabs[i].url}&amp;{$tabs[i].filter}" title="{$tabs[i].label}">{$tabs[i].label}</a>
    </td>          
    <td>&nbsp;</td>
    {/section}
  </tr>
</table>