<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage ui
 *
 * @copyright (c)2007 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Tabbed tree menu class.
 *
 * @author Yury Golovnya <yury@achievo.org>
 * @package acheivo
 * @subpackage menu
 */
class atkTabMenu
{
  /**
   * Register css and js files for menu
   *
   */
  public static function registerCssJs($page)
  {
    $theme = &atkinstance("atk.ui.atktheme");
    $plus = $theme->imgPath("plus.gif");
    $minus = $theme->imgPath("minus.gif");

    $page->register_scriptcode("
      var cookieName = 'tabmenu';
    	var ajaxRequestFile = 'tabmenu.php';
    	var plusImage = '$plus';
    	var minusImage = '$minus';
    	var plusName = 'plus.gif';
    	var minusName = 'minus.gif';
    ");
    $page->register_script(atkconfig("atkroot")."javascript/ajax.js");
    $page->register_script(atkconfig("atkroot")."javascript/folder-tree-static.js");
    $page->register_style($theme->stylePath("folder-tree-static.css"));
  }

  /**
   * Render first level menu (projects level)
   *
   * @return string html with menu list for inserting into high level template
   */
  public static function renderMenu()
  {
    $db = &atkGetDb();
    $ui = &atkinstance("atk.ui.atkui");

    $res = $db->getrows("
      SELECT
        p.id,p.name,'project' as type, p1.sub
      FROM
        project p LEFT JOIN
        (SELECT count(sub) sub, master_id
         FROM
          (SELECT
            count(id) sub,
            master_id
           FROM package
           WHERE master='project.project'
           GROUP BY master_id
           UNION
           SELECT
            count(id) sub,
            master_id
           FROM phase
           WHERE master='project.project'
           GROUP BY master_id
          ) a
         GROUP BY master_id
        ) p1 ON p.id=p1.master_id
    ");

    self::_addUrl($res,"project");
    $box = $ui->renderBox(array("tabmenu"=>$res),"treemenu");
    return $box;

  }

  /**
   * Render second level menu (packages level)
   *
   * @param integer $id parents id
   * @param string $type parents type - project/package
   * @return string html with menu list for inserting into high level template
   */
  public static function renderSubMenu($id, $type)
  {
    $db = &atkGetDb();
    $ui = &atkinstance("atk.ui.atkui");

    $res = $db->getrows("
      SELECT
        p.id,p.name,'package' as type, p1.sub
      FROM
        package p LEFT JOIN
        (SELECT count(sub) sub, master_id
         FROM
          (SELECT
            count(id) sub,
            master_id
           FROM package
           WHERE master='project.package'
           GROUP BY master_id
           UNION
           SELECT
            count(id) sub,
            master_id
           FROM phase
           WHERE master='project.package'
           GROUP BY master_id
          ) a
         GROUP BY master_id
        ) p1 ON p.id=p1.master_id
      WHERE
        p.master_id='".$id."' AND p.master='project.$type'");

    self::_addUrl($res,"package");
    $task = $db->getrows("
      SELECT
        p.id, p.name, 'phase' as type,'0' as sub
      FROM
        phase p
      WHERE
        p.master_id='".$id."' AND p.master='project.$type'");
    self::_addUrl($task,"phase");
    $res = array_merge($res,$task);
    $box = $ui->renderBox(array("tabmenu"=>$res),"treemenu");
    return $box;

  }

  /**
   * Add url, img parameter for template rendering
   *
   * @param array $rows array with menu items
   * @param string $type parents type - project/package
   */
  private function _addUrl(&$rows,$type)
  {
    $theme = &atkinstance("atk.ui.atktheme");
    for($i=0;$i<count($rows);$i++)
    {
      $rows[$i]['url'] = session_url(dispatch_url("project.".$type, "edit", array("atkselector" => $type.".id='".$rows[$i]["id"]."'"),"dispatch.php"),SESSION_NEW);
      $rows[$i]['img'] = $theme->imgPath($type.".gif");
      $rows[$i]['title'] = $rows[$i]['name'];
      $rows[$i]['name'] = substr($rows[$i]['name'],0,$theme->getAttribute("menu_item_length"));
    }
  }

}

?>