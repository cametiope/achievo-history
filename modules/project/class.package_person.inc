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
	  
	  public function postMeta()
	  {     
      $employee = $this->getAttribute("person_id");
      $employee->addListColumns("phone", "role", "email");	    
	  }
	  
	  /**
     * Override recordactions to remove the view and edit icons
     * @todo: for some reason the email action is not shown in the actionlist, fix this
     *
     * @param array $record
     * @param array $actions
     * @param array $mraactions
     */
    public function recordActions($record, &$actions, &$mraactions) 
    {
      $delete = $actions['delete']; // temp store delete action to change the order of recordlist icons
      $actions = array();
            
      if( '' != $record['person_id']['email'] )
      {
        $actions['email'] = 'javascript:document.location.href=\'mailto:'.$record['person_id']['email'].'\';';
      }
      $actions['delete'] = $delete;
    }
    
   
    /**
     * We can add employees only from the project-team 
     *
     */
    public function person_id_selection($record=array(), $mode="select")
    {
      /*@var $sm atkSessionManager*/ 
      $sm = atkGetSessionManager();
      $projectId = $sm->getValue('projectid');
      $p_attr = &$this->getAttribute('person_id');
      
      if($projectId)
      {
        list($key,$id) = explode("=",$projectId);
        $node = atkGetNode("project.project_personemployee");
        $rows = $node->selectDb("projectid=$id","","","",array("personid"));
        foreach ($rows as $row)
        {
          $ids[] = $row['personid']['id'];
        }
        if(count($ids))
        {
          $filter = "id IN(".implode(",",$ids).")";
        }
        else 
        {
          $filter = '1=0';
        }
        $p_attr->addDestinationFilter($filter);
      }
      return $p_attr->getSelectableRecords($record, $mode); 
    }
	}