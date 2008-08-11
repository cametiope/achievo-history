<table border="0" cellpadding="0" cellspacing="0" valign="top">
  <tr>
    <td width="100%" align="left">
      <br />
	    <table border="0" cellpadding="0" cellspacing="0" class="tabsTabs mainTabs">
        <tr>
          <td id="tab_project" valign="middle" align="left" nowrap="nowrap" class="passivetab">
              <a href="javascript:void(0)" onclick="showTab('project')">Project</a>

          </td>
          <td>&nbsp;</td>
		      <td id="tab_mainMenu" valign="middle" align="left" nowrap="nowrap" class="activetab">
              <a href="javascript:void(0)" onclick="showTab('main')">Navigate</a>
          </td>
          <td>&nbsp;</td>
		    </tr>
      </table>
      <table border="0" cellspacing="0" cellpadding="5" width="100%" class="tabsContent">
        <tr>
          <td>
            <div id="projectMenu">
            	<ul id="project_tree" class="dhtmlgoodies_tree">
            	{$tabmenu}
              </ul>
            </div>
            <div id="mainMenu">
            {foreach from=$menuitems item=menuitem}
            {if !isset($firstmenuitem)}{assign var='firstmenuitem' value=$menuitem.name}{/if}
              {if $menuitem.name!=='-' && $menuitem.enable}
                <a href="#" onclick="showSubMenu('{$menuitem.name|addslashes}'); window.open('{$menuitem.url}','main','');" onmouseover="this.style.cursor = 'pointer'" class="menuitem_link">
                  <div id="mi_{$menuitem.name}" class="menuItemLevel1">
                    <span class="menu-menuitem">{$menuitem.name}</span>
                  </div>
                </a>
              {/if}

              {if (count($menuitem.submenu)>0)}
                <div id="smi_{$menuitem.name}" class="submenuHover">
                  {$menuitem.header}
                  {foreach from=$menuitem.submenu item=submenuitem}
                     {if $submenuitem.enable && $submenuitem.name!=='-'}
                       <a class="menuItemLevel2" onclick="window.open('{$submenuitem.url}','main','')" onmouseover="this.style.cursor = 'pointer'; this.style.color = '#9a1010';" onmouseout="this.style.color = '#414141';">
                         {$submenuitem.name}
                       </a>
                     {/if}
                  {/foreach}
                </div>
              {/if}
            {/foreach}
            </div>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>

<script type="text/javascript">
{literal}
var prevSelectedMenu = '';
var curSelectedMenu='';


function showSubMenu(menuitemname)
{
  prevSelectedMenu = curSelectedMenu;
  hideAllSubMenus();

  if (menuitemname!==prevSelectedMenu)
  {
    curSelectedMenu = menuitemname;
    displaySubMenu(menuitemname);
  }
  else
  {
    curSelectedMenu = '';
  }
}

function displaySubMenu(menuitemname)
{
  var tags = document.getElementsByTagName("div");

  for (i = 0; i < tags.length; i++)
	{
		var id = tags.item(i).id;

		if (id=='mi_'+menuitemname)
		{
 		  tags.item(i).className='menuItemLevel2Head';
		}
	}

  submenu = document.getElementById('smi_'+menuitemname);
  if (submenu)
  {
    if (submenu.style.display =='')
      submenu.style.display = 'none';
    else
      submenu.style.display = '';
  }
}

function hideAllSubMenus()
{
  var tags = document.getElementsByTagName("div");

  for (i = 0; i < tags.length; i++)
	{
		var id = tags.item(i).id;

		if (id.substring(0,4)=="smi_")
		{
 		  tags.item(i).style.display="none";
		}
		else if (id.substring(0,3)=="mi_")
		{
 		  tags.item(i).className="menuItemLevel1";
		}
	}
}

var idOfFolderTrees = ['project_tree'];

function showProjectMenu(){
    document.getElementById('projectMenu').style.display="";
    document.getElementById('mainMenu').style.display="none";
    document.getElementById('tab_project').className = 'activetab';
    document.getElementById('tab_mainMenu').className = 'passivetab';
}

function showMainMenu(){
    document.getElementById('projectMenu').style.display="none";
    document.getElementById('mainMenu').style.display="";
    document.getElementById('tab_mainMenu').className = 'activetab';
    document.getElementById('tab_project').className = 'passivetab';
}

function showTab(tab){
  if((typeof(tab)=='undefined') || !tab){
	  tab = getTab();
	  if (!tab){
      tab = 'project';
    }
  }
	setTab(tab);

  if(tab == "main") showMainMenu();
  else showProjectMenu();
}

function getTab(){
  return parent.document.menutab;
}

function setTab(value){
  parent.document.menutab = value;
}

showTab()
{/literal}
showSubMenu('{if $atkmenutop!=="main"}{$atkmenutopname|addslashes}{else}{$firstmenuitem|addslashes}{/if}');
</script>