      
      
      
      <tr>
        <td colspan="2">
        
          <table border="0" width="100%">
            <tr>
              <td width="50">{$fields[0].full}</td>
              <td>{atktext id="updatestyle"}<br />{$fields[1].full}</td>
              <td>{$fields[3].full}<br />{$fields[2].full}</td>
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
  {section name=i loop=$fields start=4}
    {assign var=field value=$fields[i]}
    {if $field.column != 1}{include file="theme:field.tpl" field=$field}{/if}
  {/section}
          </table>
        </td>
        <td valign="top">
          <table>
  {section name=i loop=$fields start=4}
    {assign var=field value=$fields[i]}
    {if $field.column == 1}{include file="theme:field.tpl" field=$field}{/if}
  {/section}
          </table>
        </td>
      </tr>  