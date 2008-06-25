<?php

  atkimport("atk.atkmetanode.inc");

  /**
	 * Package_person
   *	
   * Package_person node used for link package and person.
	 *
 	 * @package achievo
 	 * @subpackage modules
 	 *
 	 * @author yury <yury@achievo.org>
 	 *
 	 * @copyright (c) 2008 Ibuildings.nl BV
 	 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
 	 *
 	 * @version $Revision$
 	 * $Id$
 	 */
	
	class package_person extends atkMetaNode
	{
	  public $securityAlias = "project.project";
	  
    /**
     * Meta.
     *
     * @param atkMetaPolicy $policy
     */
    public static function meta(atkMetaPolicy $policy)
	  {
	    $policy->hasOne("project.package",array("source"=>"package_id"));
	    $policy->hasOne("employee.employee",array("source"=>"person_id"));
	  }
	}