<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * This file is the skeleton menu loader, which you can copy to your
   * application dir and modify if necessary. By default, it checks
   * the menu settings and loads the proper menu.
   *
   * @package atk
   * @subpackage skel
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Peter C. Verhage <peter@ibuildings.nl>
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 1169 $
   * $Id: menu.php 1169 2005-11-15 22:07:51Z ivo $
   */

  /**
   * @internal includes
   */
  $config_atkroot = "./";
  include_once("atk.inc");

  atksession();
  atksecure();

  if(isset($_GET['parentId']))
  {
    $output = &atkOutput::getInstance();

    atkimport("menu.atktabmenu");

    $output->output(atkTabMenu::renderSubMenu($_GET['parentId'],$_GET['parentType']));

    $output->outputFlush();
  }
?>