<?php

  /**
   * Module Project
   * Class project_personemployee
   *
   * This class makes it possible to relate employees to a project in a certain role(s).
   *
   * $Id$
   * @author L. Willems  <lineke@ibuildings.nl>
   * @version $Revision$
   *
   */

  userelation("atkmanytoonerelation");
  useattrib("project.dummyeditattribute");

  /**
   * Constructor.
   *
   */
  class project_personemployee extends atkNode
  {
    public function __construct()
    {
      $this->atkNode("project_personemployee");

      $this->add(new atkManyToOneRelation("projectid", "project.project", AF_OBLIGATORY|AF_PRIMARY|AF_HIDE));
      $this->add(new atkManyToOneRelation("personid", "employee.employee", AF_OBLIGATORY|AF_PRIMARY|AF_LARGE))->
        addListColumns("department","email","phone");
      $this->add(new atkManyToOneRelation("role", "project.role", AF_PRIMARY|AF_HIDE_EDIT|AF_HIDE_VIEW|AF_HIDE_ADD));
      $this->addFilter("personid.role = 'employee'");
    
      //$this->addFilter("project_person.projectid", atkGetPostVar('projectid'));

      $this->add(new atkDummyAttribute("hint", atktext("project_member_fuzzyhint"), AF_HIDE_LIST|AF_HIDE_VIEW|AF_HIDE_EDIT));
      $this->add(new dummyEditAttribute("member"));
      // If you may edit the project, you may also add contactpersons to it.
      $this->setSecurityAlias("project.project");
      foreach(array("add", "save", "update", "delete") as $action)
        $this->m_securityMap[$action] = "edit";

      $this->setTable("project_person", "project_person");
    
      $this->setDescriptorTemplate('[personid.lastname], [personid.firstname] ([personid.userid])');
    }

    function action_delete(&$handler)
    {
      // When deleting, the above filter won't work. But that doesn't matter, we
      // don't need it anyway.
      $this->m_flags |= NF_NO_FILTER;
      return $handler->action_delete();
    }

    /**
     * Overwrite personid edit for show employee and role in one line.
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     * @return string
     */
    function personid_edit($record="", $fieldprefix="", $mode="")
    {
      return $this->getAttribute("personid")->edit($record, $fieldprefix, $mode)." ".$this->getAttribute("role")->edit($record, $fieldprefix, $mode);
    }

    /**
     * Overwrite personid display for show employee and role in one line.
     *
     * @param array $record
     * @param string $mode
     * @return string
     */
    function personid_display($record, $mode="")
    {
      return $this->getAttribute("personid")->display($record, $fieldprefix, $mode)." ".$this->getAttribute("role")->display($record, $fieldprefix, $mode);
    }

    /**
     * Save a new record to the database.
     *
     * The record is passed by reference, because any autoincrement field gets
     * its value when stored to the database. The record is updated, so after
     * the call to addDb you can use access the primary key fields.
     *
     * @param array $record The record to save.
     * @param boolean $exectrigger Indicates whether the postAdd trigger
     *                             should be fired.
     * @param array $excludelist List of attributenames that should be ignored
     *                           and not stored in the database.
     * @return boolean True if succesful, false if not.
     */
    public function addDb(&$record, $exectrigger=true, $mode="add", $excludelist=array())
    {
      if(atkArrayNvl($record,"member"))
      {
        return  true;
      }
      else
      {
        return parent::addDb($record, $exectrigger, $mode, $excludelist);
      }
    }
  
    private function addMember(&$record)
    {
      $tokens = explode(",", $record["member"]);
      $node = &atkGetNode("employee.employee");
      $result = false;
      foreach ($tokens as $token)
      {
        $token = trim($token);
        if($token)
        {
          $found = $node->searchDb($token);
          if(count($found) == 0)
          {
           $string = $this->text("no_member_error");
            atkimport("atk.utils.atkstringparser");   
           $parser = new atkStringParser($string);
           $msg = $parser->parse(array("param"=>$token));
          
            triggerError($record, "member", 'no_member_error',$msg);
          }
          elseif(count($found)>1)
          {
            $string = $this->text("member_many_match_error");
            atkimport("atk.utils.atkstringparser");   
            $parser = new atkStringParser($string);
            $msg = $parser->parse(array("param"=>$token));
          
            triggerError($record, "member", 'member_many_match_error',$msg);
          }
          else 
          {
            $record["personid"] = $found[0];
            if(parent::addDb($record, $exectrigger, $mode, $excludelist))
            {
              $result = true;
            }
          }
        }
      }
      if(!$result && count($record['atkerror']) == 0)
      {
        triggerError($record, "member", 'member_match_not_found_error');
      }
      return $result;
    }
  
    /**
     * Validates a record.
     *
     * Validates unique fields, required fields, dataformat etc.
     *
     * @internal This method instantiates the node's validator object, and
     *           delegates validation to that object.
     *
     * @param array $record The record to validate
     * @param String mode The mode for which validation is performed ('add' or 'update')
     * @param array ignoreList The list of attributes that should not be
     *                         validated
     */
    public function validate(&$record, $mode, $ignoreList=array())
    {
      if(atkArrayNvl($record,"member"))
      {
        return  $this->addMember($record);
      }
      return parent::validate($record, $mode, $ignoreList);
    }

  }