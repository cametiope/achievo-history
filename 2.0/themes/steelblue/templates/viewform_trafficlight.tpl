    <table cellpadding="3">

      <tr>
        <td colspan="2">
        
        
          <table border="0" width="100%">
            <tr>
              <td width="50">{$fields[0].full}</td>
              <td>{$fields[2].full}</td>
            </tr>
          </table>        

        </td>
      </tr>

      <tr>
        <td colspan="2">&nbsp;</td>
      </tr>

      <tr>
        <td valign="top">
          <table>
          {section name=i loop=$fields start=3}
          {assign var=field value=$fields[i]}
          {if $field.column != 1}
            <tr{if $field.rowid != ""} id="{$field.rowid}"{/if}{if !$field.initial_on_tab} style="display: none"{/if} class="{$field.class}">
              {if isset($field.line)}
                <td colspan="2" valign="top" class="field">{$field.line}</td>
              {else}
                {if $field.label!=="AF_NO_LABEL"}<td valign="top" class="fieldlabel">{if $field.label!=""}{$field.label}: {/if}</td>{/if}
                <td valign="top" class="field" {if $field.label==="AF_NO_LABEL"}colspan="2"{/if}>{$field.full}</td>
              {/if}
            </tr>
          {/if}
          {/section}
          </table>
        </td>
        <td valign="top">
          <table>
          {section name=i loop=$fields start=3}
          {assign var=field value=$fields[i]}
          {if $field.column == 1}
            <tr{if $field.rowid != ""} id="{$field.rowid}"{/if}{if !$field.initial_on_tab} style="display: none"{/if} class="{$field.class}">
              {if isset($field.line)}
                <td colspan="2" valign="top" class="field">{$field.line}</td>
              {else}
                {if $field.label!=="AF_NO_LABEL"}<td valign="top" class="fieldlabel">{if $field.label!=""}{$field.label}: {/if}</td>{/if}
                <td valign="top" class="field" {if $field.label==="AF_NO_LABEL"}colspan="2"{/if}>{$field.full}</td>
              {/if}
            </tr>
          {/if}
          {/section}
          </table>
        </td>
      </tr>
      
    </table>