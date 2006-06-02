<?php

atkimport("atk.menu.atkplainmenu");

class achievo_modern extends atkPlainMenu 
{
  function render()
  {
    global $ATK_VARS,$g_menu, $g_menu_parent;
    $atkmenutop="main";
    
    if (is_array($g_menu[$atkmenutop]))
    {
      usort($g_menu[$atkmenutop],array("atkplainmenu","menu_cmp"));
      $menuitems = array();
      foreach ($g_menu[$atkmenutop] as $menuitem)
      {
        $this->setEnabled($menuitem);

        // submenu
        if ($g_menu[$menuitem['name']])
        {
          $enablesubmenu=false;
          for ($i=0;$i<count($g_menu[$menuitem['name']]);$i++)
          {
            if ($this->setEnabled($g_menu[$menuitem['name']][$i]))
            {
              $enablesubmenu = true;
            }
          }
          if ($enablesubmenu) $menuitem['submenu'] = $g_menu[$menuitem['name']];
        }
        $menuitems[] = $menuitem;
      }
    }

    $page = &atknew("atk.ui.atkpage");
    $theme = &atkinstance("atk.ui.atktheme");
    $page->register_style($theme->stylePath("style.css"));
    $page->register_style($theme->stylePath("menu.css"));
    $page->register_script(atkconfig("atkroot")."atk/javascript/menuload.js");

    $ui = &atkinstance("atk.ui.atkui");

    $box = $ui->renderBox(array("title"=>$this->getMenuTranslation($atkmenutop,$modname),
                                "menuitems"=>$menuitems,
                                'menutop'=>$atkmenutop,
                                'g_menu'=>$g_menu),"menu");

    $page->addContent($box);

    return $page->render("Menu", true);
  }
  
  function setEnabled(&$menuitem)
  {
    if (is_array($menuitem["enable"]))
    {
      $enabled = false;
      for ($j=0;$j<(count($menuitem["enable"])/2);$j++)
      {
        $enabled |= is_allowed($enable[(2*$j)],$enable[(2*$j)+1]);
      }
      $menuitem["enable"]=$enabled;
    }
    else 
      $enabled = $menuitem["enable"];
      
    return ($menuitem["name"]==='-'?false:$enabled);
  }

  function getHeader($atkmenutop)
  {
    if ($atkmenutop == "projectmanagement")
    {
      global $g_sessionManager;
      include_once "achievotools.inc";
      $projects = $g_sessionManager->getValue("selectedprj","globals");

      if (count($projects) == 0)
      {
        //updateSelectedProjects();
        $projects = $g_sessionManager->getValue("selectedprj","globals");
      }

      $prj  = atktext("project_select").":";
      $prj .="<FORM><SELECT style=\"width: 150px\" name=\"selectedproject\" onchange=\"reloadProjects(this);\">";
      $prj .= "<OPTION value=\"0\">".atktext("project_select_none")."</OPTION>";

      $selectedproject = array_key_exists("selectedproject", $_REQUEST) ? $_REQUEST["selectedproject"] : null;
      for ($i=0;$i < count($projects); $i++)
      {
        $prj .= "<OPTION value=\"".$projects[$i]['projectid']."\"";
        if ($selectedproject == $projects[$i]['projectid']) $prj .=" selected";
        $prj .= ">".$projects[$i]['projectname']."</OPTION>";
      }
      $prj .="</SELECT></FORM>";
      return $prj;

    }
  }
}

?>