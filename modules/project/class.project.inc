<?php
  include_once("achievotools.inc");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  userelation("atkmanyboolrelation");
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("atkboolattribute");
  useattrib("project.dependencyattrib");
  useattrib("project.projecttemplateattribute");
  useattrib("atknumberattribute");
  useattrib("graph.datagraphattribute");
  useattrib("atkfuzzysearchattribute");
  useattrib("atkdummyattribute");
  useattrib("atknumberattribute");
  useattrib("docmanager.projectdocumentmanager");
  useattrib("project.trafficlightattribute");
  useattrib("project.trafficlightmemoattribute");

  define("PRJ_TIMEREG_ALL_USERS", 0);
  define("PRJ_TIMEREG_MEMBERS_ONLY", 1);

  class project extends atkNode
  {

    /**
     * Project identifier
     *
     * @var string
     */
    private $project;

    
    /**
     * Memo attribute object
     *
     * @var object
     */
    private $memo_attr;
      
     
    /**
     * Constructor
     *
     * @param string $name
     * @param integer $flags
     */
    public function project($name="project", $flags=0)
    {
      global $ATK_VARS, $g_sessionManager;

      $this->atkNode($name,$flags|NF_ADD_LINK|NF_EDITAFTERADD|NF_IMPORT);
      if($name=="project")
      {
        $listener = &atknew("modules.crm.listeners.eventlistener");
        $this->addListener($listener);
      }

      $this->setProjectId();

      $this->add(new atkNumberAttribute("id",AF_AUTOKEY));

      // The next three attributes have to be the first three attributes in this node
      $this->add(new trafficLightAttribute('trafficlight', AF_HIDE_LIST));
      $radio = &$this->add(new atkRadioAttribute('trafficlight_updatestyle', array('manual', 'automatic'), array('M', 'A'), AF_FORCE_LOAD|AF_HIDE_LIST|AF_HIDE_ADD|AF_DISPLAY_VERT));
      
      // Hide attribute and add it as a class member var so we can manipulate it later
      $memo = &$this->add(new trafficLightMemoAttribute('trafficlight_memo', array('rows' => '4'), AF_HIDE|AF_FORCE_LOAD));      
      $this->memo_attr = $memo;
            
      $radio->addDependee('trafficlight_selection');
      $radio->addDependee('trafficlight_memo_field');
      $this->add(new atkDummyAttribute('trafficlight_selection', '', AF_NO_LABEL|AF_HIDE_LIST|AF_HIDE_ADD));     
      $this->add(new atkDummyAttribute('trafficlight_memo_field', '', AF_NO_LABEL|AF_HIDE_LIST|AF_HIDE_ADD));     
              
      $this->add(new atkAttribute("abbreviation", AF_SEARCHABLE|AF_UNIQUE, 10));
      $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE, 200));
      $this->add(new atkManyToOneRelation("project_category","project.project_category",AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK));
      $this->add(new atkManyToOneRelation("project_group_id","project.project_group",AF_HIDE_LIST|AF_HIDE_ADD));

      $this->add(new atkManyToOneRelation("coordinator","employee.employee",AF_MANYTOONE_AUTOCOMPLETE|AF_SEARCHABLE|AF_HIDE_ADD|AF_RELATION_AUTOLINK))
      	->setDestinationFilter("person.status='active'");

      $this->add(new atkManyToOneRelation("contract_id","organization.contracts",AF_MANYTOONE_AUTOCOMPLETE|AF_HIDE_ADD|AF_HIDE_LIST));

      if(atkConfig::get("project","use_subprojects"))
      {
        $p_attr = &$this->add(new atkManyToOneRelation("master_project","project.project",AF_RELATION_AUTOCOMPLETE|AF_HIDE_LIST|AF_HIDE_ADD));
        $p_attr->addDestinationFilter("project.id<>[id] AND (project.master_project=0 OR project.master_project IS NULL)");
      }

      $this->add(new atkTextAttribute("description",TEXT_LARGE|AF_HIDE_LIST|AF_HIDE_ADD));

      $this->add(new atkAttribute("quotation_number", AF_HIDE_LIST|AF_HIDE_ADD));

      /** @todo: Remove filters from project node constructor */
      if (in_array($ATK_VARS["atknodetype"], array("project.project", "project.project_personcontact", "project.project_personemployee", "project.phase", "search.search"))&& in_array($ATK_VARS["atkaction"],array("admin","add","save","update","edit","search", "view")) || $reportnodetype=="hoursurvey")
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE_ADD|AF_LIST_NO_NULL_ITEM|AF_HIDE_LIST, 15));
      }
      else
      {
        $this->add(new atkListAttribute("status",array("active","nonactive","archived"),array(),AF_HIDE|AF_LIST_NO_NULL_ITEM, 15));
        $this->addFilter("project.status='active'");
      }

      // Add the organization relation
      $this->add(new atkManyToOneRelation("customer", "organization.organization", AF_HIDE_LIST|AF_RELATION_AUTOLINK|AF_HIDE_ADD|AF_SEARCHABLE|AF_MANYTOONE_AUTOCOMPLETE|AF_FORCE_LOAD));
      $this->add(new atkDateAttribute("startdate","","",0,0,AF_OBLIGATORY|AF_FORCE_LOAD));
      $this->add(new atkDateAttribute("enddate","","",0,0,AF_OBLIGATORY|AF_FORCE_LOAD));
      $this->add(new atkDateAttribute("initial_enddate","","",0,0,AF_HIDE));
      $this->add(new atkListAttribute("timereg_limit", array("all_users", "members_only"), array(PRJ_TIMEREG_ALL_USERS, PRJ_TIMEREG_MEMBERS_ONLY), AF_LIST_NO_NULL_ITEM|AF_HIDE_ADD|AF_HIDE_LIST));

      $this->add(new atkNumberAttribute("completed",AF_READONLY|AF_HIDE_ADD));

      // Time registration
      $this->add(new atkDurationAttribute('initial_planning', '15m', '100h', AF_READONLY|AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkDurationAttribute('current_planning', '15m', '100h', AF_READONLY|AF_HIDE_LIST|AF_HIDE_ADD));

      // $this->add(new spendHoursAttribute("spend_hours","id",AF_TOTAL));
      $this->add(new atkDurationAttribute('spend_hours', '15m', '100h', AF_READONLY|AF_HIDE_LIST|AF_HIDE_ADD));

      $this->add(new atkFieldSet('actual_planning', '[current_planning.field] &nbsp; ' . $this->text('booked') . ': [spend_hours.field]', AF_HIDE_LIST|AF_HIDE_ADD));

      $this->add(new atkDurationAttribute('remaining_effort_time', '15m', '100h', AF_READONLY|AF_HIDE_LIST|AF_HIDE_ADD));

      $this->add( new atkBoolAttribute( 'description_obligatory', AF_HIDE_ADD|AF_FORCE_LOAD ) );
      $this->add( new atkBoolAttribute( 'use_activities', AF_HIDE_ADD|AF_FORCE_LOAD ) );     
      
      // If we are in project administration mode, we show all projects. Otherwise, we only
      // show active projects.
      if (!isset($_SERVER["PWD"]))
        $reportnodetype = $g_sessionManager->stackVar("reportnodetype");
      else
        $reportnodetype = "";
      
      if( atkGetPostVar('atkaction') != 'add' && atkGetPostVar('atkprevaction') != 'add' )
      { 
        $deliverable = &$this->add(new atkOneToManyRelation("deliverable", "project.deliverable","project_id", AF_NO_LABEL|AF_HIDE_LIST|AF_CASCADE_DELETE));
        $deliverable->setDestinationFilter('project_id=[id]');
      }
      $this->setColumnAttribs(array('deliverable'));

      $this->setTable("project","project");

      $this->setOrder('id', 'trafficlight', 'trafficlight_updatestyle', 'trafficlight_memo', 'trafficlight_selection', 'trafficlight_memo_field');
      $this->setIndex("name");
      
      // @todo: commented because these attributes aren't needed in the General tab.
      //if (($ATK_VARS['atkaction'] == "edit" || $ATK_VARS['atkaction'] == "update") && atkconfig("project_contact_obligatory"))
      //{
      //  $flags = AF_OBLIGATORY;
      //}
      //else
      //{
      //  $flags = 0;
      //}
      //$this->add(new atkOneToManyRelation("contacts","project.project_personcontact", "projectid", AF_HIDE_LIST|AF_CASCADE_DELETE|AF_HIDE_ADD|$flags));
      //$this->add(new atkOneToManyRelation("members","project.project_personemployee", "projectid", AF_HIDE_LIST|AF_CASCADE_DELETE), "planning");
      //$this->add(new atkDummyAttribute("hint", atktext("project_member_fuzzyhint"), AF_HIDE_LIST|AF_HIDE_VIEW|AF_HIDE_ADD), "planning");
      //$this->add(new atkFuzzySearchAttribute("member_add", "employee.employee", "storeMembers", "multiselect", AF_HIDE_ADD|AF_BLANK_LABEL), "planning");
      //$this->add(new atkBoolAttribute("alwaysvisibleintimereg", AF_HIDE_LIST|AF_HIDE_ADD), "planning");
      //$this->add(new atkDummyAttribute("separator", "<br><hr><br>", AF_HIDE_LIST|AF_HIDE_ADD), "planning");
      //$this->add(new atkOneToManyRelation("deliverables", "project.deliverable", "project_id", AF_HIDE_LIST), "planning");
      //$p_attr = &$this->add(new atkManyToOneRelation("master_project","project.project",AF_RELATION_AUTOCOMPLETE|AF_HIDE_LIST|AF_HIDE_ADD));
      //$p_attr->addDestinationFilter("project.id<>[id] AND (project.master_project=0 OR project.master_project IS NULL)");
      //$this->add(new atkNumberAttribute("fixed_price",AF_HIDE_LIST|AF_HIDE_ADD, 13, 2), "finance");
      //all phases (including manyally added ones) are shown in this relation.
      //$this->add(new atkOneToManyRelation("phase","project.phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE),"planning");
      //$this->add(new dependencyAttribute("dependencies",AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANKLABEL|AF_HIDE_VIEW), "planning");
      //$this->add(new projecttemplateAttribute("template","project.tpl_project", AF_HIDE_LIST|AF_HIDE_EDIT|AF_FORCE_LOAD|AF_HIDE_SEARCH|AF_HIDE_VIEW));
      //$this->add(new atkOneToManyRelation("todos", "todo.todo", "projectid", AF_HIDE_LIST|AF_HIDE_SEARCH), "todos");
      //$this->add(new atkOneToManyRelation("notes", "notes.project_notes", "projectid", AF_HIDE_LIST|AF_HIDE_SEARCH), "notes");
      //$this->add(new projectDocumentManager("documents", AF_HIDE_LIST|AF_HIDE_SEARCH), "documents");
      //$timeline = &new dataGraphAttribute("timeline", "timeline", array("projectid"=>"[id]", "resolution"=>"auto"), "line", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT);
      //$timeline->addDynamicParam("resolution", array("day", "week", "month", "auto"));
      //$this->add($timeline, "stats");
      //$this->add(new dataGraphAttribute("phasetimedistribution", "phasetime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");
      //$this->add(new dataGraphAttribute("emptimedistribution", "emptime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");
      //$this->add(new dataGraphAttribute("activitytimedistribution", "activitytime", array("projectid"=>"[id]"), "auto", AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_EDIT), "stats");
      //if(atkconfig::get("timereg","timereg_contact_link",false))
      //{
        //$this->add(new atkListAttribute("contact_restrict",array("restrict_project","restrict_eligible"),array(1,2),AF_HIDE_LIST|AF_FORCE_LOAD|AF_OBLIGATORY));
      //}

      //$tmp = &new atkManyBoolRelation("phasetemplatematrix","project.phase","project.tpl_phase", AF_HIDE_LIST|AF_HIDE_SEARCH);
      //$tmp->m_localKey = "projectid";
      //$tmp->m_remoteKey = "template";
      //$tmp->setStoreDeletionFilter("phase.template IS NOT NULL AND phase.template!='' AND phase.template!=0");
      //The phasetemplatematrix is in add mode visible on tab 'default' and in other modes on the 'planning' tab
      //$tab = ($ATK_VARS["atkaction"]=="view"||$ATK_VARS["atkaction"]=="edit"||$ATK_VARS["atkaction"]=="update"?"planning":"default");
      //$this->add($tmp, $tab);
    }

    /**
     * Generate output to show or hide a selectlist for the trafficlight attribute
     * 
     * @param array
     * @param string
     * @return string
     */
    public function trafficlight_selection_edit( &$record )
    {
      $output = '<div id="project_project_trafficlight_selection">';
      if( isset( $record['trafficlight_updatestyle'] ) && $record['trafficlight_updatestyle'] == 'M' )
      {       
        $red    = atkConfig::get('project', 'trafficlight_red');
        $yellow = atkConfig::get('project', 'trafficlight_yellow'); 
        $green  = atkConfig::get('project', 'trafficlight_green'); 
        
        $output.= '<select name="trafficlight" '.(!$record['trafficlight'] ? 'selected' : '' ).'>';
        $output.= '<option value="">'.atktext('list_null_value_obligatory').'</option>';
        $output.= '<option value="'.$red['start'].'">'.atktext('red').'</option>';
        $output.= '<option value="'.$yellow['start'].'">'.atktext('yellow').'</option>';
        $output.= '<option value="'.$green['start'].'">'.atktext('green').'</option>';
       
        $output.= '</select></div><br />';
      }     
      return $output;
    }    
    
    
    /**
     * Generate output to change the memo field attribute depending on the selected updatestyle
     * 
     * @param array
     * @param string
     * @return string
     */
    public function trafficlight_memo_field_edit( &$record, $fieldprefix = '' )
    {
      $output = '<div id="project_project_trafficlight_memo_field">';
      $this->memo_attr->setForceUpdate( true );
      
      if( isset( $record['trafficlight_updatestyle'] ) && $record['trafficlight_updatestyle'] == 'A' )
      {  
        // Get memo attribute's content by executing a query
        // This value should probably come directly from $record, but somehow this wont work
        $db = &atkGetDb();
        $row = $db->getRows('SELECT trafficlight_memo FROM project WHERE id = '.(int) $record['id']);
        $record['trafficlight_memo'] = $row[0]['trafficlight_memo'];
        
        $this->memo_attr->addFlag( AF_READONLY );      
        $output .= '<textarea cols="70" rows="4" disabled="disabled" name="trafficlight_memo">';
        $output .= $record['trafficlight_memo'];
        $output .= '</textarea>';
      }
      else
      {
        $this->memo_attr->removeFlag( AF_READONLY );
        $output .= $this->memo_attr->getEdit( 'edit', $record, $fieldprefix );        
      }      

      $output .= '</div>';
      return $output;
    }
        
    function graph_topprojects($params)
    {
      $db = &atkGetDb();

      $start = $params["startdate"];
      $end   = $params["enddate"];
      $max   = $params["max"];
      $from  = $params["from"];

      $query = &$db->createQuery();

      $query->addTable("hours");
      $query->addJoin("phase", "", "phase.id = hours.phaseid", false);
      $query->addJoin("project", "", "phase.projectid = project.id", false);
      $query->addField("project.name");
      $query->addField("project.id");
      $query->addField("sum(time) as totaltime");
      $query->addCondition("hours.activitydate BETWEEN '".$start."' AND '".$end."'");
      if ($max)
      {
        $query->setLimit($from,$max);
      }
      $query->addOrderBy("totaltime DESC");
      $query->addGroupBy("project.name");
      $data = $db->getrows($query->buildSelect());

      // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["name"]] = $data[$i]["totaltime"];
      }
      return array(atktext("registeredtimeperproj")=>$dataset);
    }

    // We override the dispatch function, to intercept projectid's. We use this
    // to update the list of recent projects etc. So whatever action you perform
    // on the project node, the current project you are using (if any) is stored
    // in your history. (exceptions: actions add and delete do not add
    // the project to the recent list, for obvious reasons)
    function dispatch($postvars, $fullpage=true)
    {
      if ($postvars['atkaction']!='add' && $postvars['atkaction']!='delete')
      {
        updateSelectedProjects();
      }
      return parent::dispatch($postvars, $fullpage);
    }

    function descriptorFields()
    {
      return array("abbreviation", "name");
    }

    function descriptor($record)
    {
      if (empty($record["abbreviation"]))
        return $record["name"];
      else
        return sprintf("%s: %s", $record["abbreviation"], $record["name"]);
    }

    function postDelete($record)
    {
      $eventlog = &atkGetNode("crm.eventlog");
      $eventlog->delete_event_history($this->atknodetype(),$record["atkprimkey"]);
      return true;
    }

    function getInitialProjectcode()
    {
      return "";
    }


    function initial_values()
    {
      $projectnumbermodule = atkConfig('projectnumbermodule');
      if (!empty($projectnumbermodule))
      {
        atkdebug("Getting alternative project code.");
        $numberingmodule = &getModule($projectnumbermodule);
        $initialprojectcode = $numberingmodule->getInitialProjectcode();
      }
      else
      {
        atkdebug("Getting projectcode.");
        $initialprojectcode = $this->getInitialProjectcode();
      }

      $nextyear = strtotime("+1 year");
      return array("abbreviation" => $initialprojectcode,
                   "startdate" => array("year"=>date("Y"),
                                        "month"=>date("m"),
                                        "day"=>date("d")),
                   "enddate" => array("year"=>(date("Y",$nextyear)),
                                      "month"=>date("m",$nextyear),
                                      "day"=>date("d",$nextyear)),
                   "timereg_limit" => PRJ_TIMEREG_MEMBERS_ONLY,
                   "status" => "active",
                   "completed" => 0,
                   "initial_planning" => 0,
                   "current_planning" => 0,
                   "remaining_effort_time" => 0,
                   "coordinator"=>getUser(),
                   "trafficlight_updatestyle" => "A",);
    }

    /**
     * Display override for completed field
     *
     * @param array $record
     * @param string $mode
     * @return string
     */
    public function completed_display($record,$mode)
    {
      return $record['completed'].'%';
    }

    function action_billsetup(&$handler)
    {
      include_once moduleDir("finance").'billsetup.inc';
    }

    function action_billing(&$handler)
    {
      include_once moduleDir("finance").'projectbilling.inc';
    }

    function handleTemplateMatrix($record)
    {
      //when in add mode, if no phases were selected in the matrix, we do
      //not need to update.
      if(count($record["phasetemplatematrix"])==0)
      {
        atkdebug("no matrix template items selected in mode add.");
        return true;
      }

      $this->updatePhaseFields($record, "add");
      return true;
    }

    /**
     * One line description of the function
     *
     * Full description of the function
     *
     * @param type name description
     * @return type description
     */
    function postAdd($rec)
    {
			$this->addCoordinatorToTeam($rec);
    	$this->handleTemplateMatrix($rec);

      $skel = atkconfig('project_dir_skel');
      $dest = atkconfig('project_dir_destination');

      if ($skel != '' && $dest != '' && is_dir($skel))
      {
        atkdebug("Copying skell: $skel to destination: $dest started.");
        atkimport("atk.utils.atkfileutils");


        if (!is_dir($dest))
        {
          atkdebug("Destination isn't a existing directory. First create the destination.");
          $succes = atkFileUtils::mkdirRecursive($dest);
        }
        else
        {
          atkdebug("Destination is a existing directory.");
          $succes = true;
        }

        $dirname = atkFileUtils::parseDirectoryName(atkconfig('project_dir_name_template'), $rec);

        if (atkFileUtils::copyDirRecursive($skel, $dest, $dirname) && $succes)
        {
          atkdebug("The skell is copied to the destination.");

          $mailer = &atknew('atk.utils.atkmailer');

          $body = sprintf(atktext("project_body"), $rec['abbreviation'], getcwd()."/".$dest."/".$dirname);

          switch (atkconfig("project_formatmail"))
          {
            default:
              break;

            case 'html':
              $mailer->isHTML(true);
              break;

            case 'htmlplain':
              $mailer->isHTML(true);
              $mailer->AltBody = strip_tags($body, '<a>');
              break;
          }

          $mailer->From = atkconfig("mail_sender", "achievo");
          $mailer->AddAddress(atkconfig("project_sendto"));
          $mailer->Subject = atktext("project_subject").$rec['abbreviation'];
          $mailer->Body = $body;
          $mailer->Send();
          atkdebug("Mail send to ".atkconfig("project_sendto")."?");
        }
        else
          atkerror("No write permisson on destination: ".$dest);
      }
      else
        atkdebug("Configs are empty or skell isn't a directory.");

      return true;
    }

    function postDel($rec)
    {
      // TODO: delete phases and dependencies, and hours (???)
      // Phases, dependancies, todo's, notes, activities are permenantly deleted
      // trough the AF_CASCADE_DELETE flag
      // At the moment a project is also permanently deleted. This wil corrupt de hours survey.
      // Therefore NF_DELETED_FLAG should be realised for hours, projects and
      // employees.
      return true;
    }

    function fixed_price_edit($rec)
    {
      return $this->m_attribList["fixed_price"]->edit($rec)." ".atkconfig("currency_symbol");
    }

    function graph_phasetime($params, $raw=false)
    {
      $db = &atkGetDb();
      $data = $db->getrows("SELECT
                                phase.id, phase.name, SUM(time) as total
                              FROM
                                hours, phase
                              WHERE
                                hours.phaseid = phase.id
                                AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                            " GROUP BY
                                phase.name
                              ORDER BY
                                phase.name");

      if ($raw) return $data;

      // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["name"]] = $data[$i]["total"];
      }

      return array("registeredtimeperphase"=>$dataset);
    }

    function graph_emptime($params, $raw=false)
    {
      $db = &atkGetDb();
      $data = $db->getrows("SELECT
                                person.id, person.firstname, person.lastname, SUM(time) as total
                              FROM
                                hours, phase, person
                              WHERE
                                hours.phaseid = phase.id
                                AND hours.userid = person.id
                                AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                            " GROUP BY
                                person.id
                              ORDER BY
                                person.lastname");

      if ($raw)
      {
        return $data;
      }

       // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["firstname"]." ".$data[$i]["lastname"]] = $data[$i]["total"];
      }

      return array("registeredtimeperemp"=>$dataset);
    }

    function graph_activitytime($params, $raw=false)
    {
      $db = &atkGetDb();
      $data = $db->getrows("SELECT
                                activity.id, activity.name, SUM(time) as total
                              FROM
                                hours, phase, activity
                              WHERE
                                hours.phaseid = phase.id
                                AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                            "   AND hours.activityid = activity.id
                              GROUP BY
                                activity.name
                              ORDER BY
                                activity.name");
      // In raw mode the data is sufficient.
      if ($raw) return $data;

      // convert records to graph-compatible array.
      $dataset = array();

      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["name"]] = $data[$i]["total"];
      }

      return array("registeredtimeperactivity"=>$dataset);
    }

    function graph_dowtime($params, $raw=false)
    {
      $db = &atkGetDb();

      $days = array("sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday");

      $data = $db->getrows("SELECT
                                date_format(hours.activitydate, '%w') as dow, SUM(time) as total
                              FROM
                                hours, phase, activity
                              WHERE
                                hours.phaseid = phase.id
                                AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
                            "   AND hours.activityid = activity.id
                              GROUP BY
                                date_format(hours.activitydate, '%w')
                              ORDER BY
                                dow");

      // Add weekday names.
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $data[$i]["dow"] = atktext($days[$data[$i]["dow"]]);
      }

      // In raw mode the data is sufficient.
      if ($raw) return $data;

      // convert records to graph-compatible array.
      $dataset = array();

      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$data[$i]["dow"]] = $data[$i]["total"];
      }

      return array("registeredtimeperweekday"=>$dataset);
    }

    function graph_timeline($params)
    {
      $db = &atkGetDb();

      // First find out if a timeline would make more sense per week, per
      // month or per day. This all depends on the amount of time between the
      // first and last time entry.
      $range = $db->getrows("SELECT
                                 min(activitydate) as minimum,
                                 max(activitydate) as maximum
                               FROM
                                 hours, phase
                               WHERE
                                 hours.phaseid = phase.id
                                 AND phase.projectid = ".$params["projectid"].
                                ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                                ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":""));
      $maxdate = array();
      $mindate = array();
      list ($maxyear, $maxmonth, $maxday) = explode("-", $range[0]["maximum"]);
      list ($minyear, $minmonth, $minday) = explode("-", $range[0]["minimum"]);

      $secondsinaday = 24*60*60;

      $resolution = $params["resolution"];

      // Do we need to autodetect a resolution?
      if ($resolution=="auto"||$resolution=="")
      {
        $daysbetween = (adodb_mktime(12,0,0,$maxmonth,$maxday,$maxyear)-adodb_mktime(12,0,0,$minmonth,$minday,$minyear))/$secondsinaday;
        if ($daysbetween <= 31)
        {
          // one month range. Daybased stat makes the most sense.
          $resolution="day";
        }
        else if ($daysbetween <= 5*31)
        {
          // five month range. Week based stat makes the most sense.
          $resolution="week";
        }
        else
        {
          // > 5 month range. Month based stat makes most sense.
          $resolution="month";
        }
      }

      switch ($resolution)
      {
        case "month": $groupbystmt = "DATE_FORMAT(activitydate,'%Y%m')"; break;
        case "week": $groupbystmt = "DATE_FORMAT(activitydate,'%Y%V')"; break;
        default: $groupbybystmt = "";
      }

      $query = "SELECT
                  ".($groupbystmt==""?"activitydate":$groupbystmt)." as label, sum(time) as total
                FROM
                  hours, phase
                WHERE
                  hours.phaseid = phase.id
                  AND phase.projectid = ".$params["projectid"].
                  ($params["viewstart"]!=""?" AND hours.activitydate>='".$params["viewstart"]."'":"").
                  ($params["viewend"]!=""?" AND hours.activitydate<='".$params["viewend"]."'":"").
              " GROUP BY
                  ".($groupbystmt==""?"activitydate":$groupbystmt);
      $data = $db->getrows($query);

      // convert records to graph-compatible array.
      $dataset = array();
      for ($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $dataset[$this->_graphLabel($data[$i]["label"], $resolution)] = sprintf("%.2f", ($data[$i]["total"]/60));
      }

      return array("registeredtimeper".$resolution=>$dataset);

    }

    function _graphLabel($label, $scale)
    {
      if ($scale=="month")
      {
        // output nice monthnames
        $months = array("jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct",  "nov", "dec");
        $shortyear = substr($label, 2, 2);
        $month = sprintf("%d", substr($label, 4, 2));
        return atktext($months[$month-1])." '".$shortyear;
      }
      else
      {
        return $label;
      }
    }

    function member_add_edit($record, $mode)
    {
      $org = $this->m_attribList["member_add"]->edit($record, $mode);

      $rolesel = new atkManyToOneRelation("member_add_role", "project.role", AF_OBLIGATORY);

      $dummy = array();
      $org.= " ".$rolesel->edit($dummy);
      return $org;
    }

    function storeMembers($rec, $members)
    {
      $role_id = $this->fetchRoleId();

      for ($i=0, $_i=count($members); $i<$_i; $i++)
      {
        $this->_addMemberRecord($rec["id"], $members[$i]["id"], $role_id);
      }

      return true;
    }

    function fetchRoleId()
    {
      //$raw = $this->m_postvars["member_add_role"];
      $rolesel = new atkManyToOneRelation("member_add_role", "project.role", AF_OBLIGATORY);
      $role_id = $rolesel->fetchValue($this->m_postvars);
      if($role_id["id"]!="")
      {
        return $role_id["id"];
      }
      else
      {
        return 0;
      }
    }

    function _addMemberRecord($project_id, $person_id, $role_id)
    {
      $db = &atkGetDb();

      // attendee may already exist.
      $recs = $db->getrows("SELECT count(*) AS cnt FROM project_person WHERE projectid = $project_id AND personid = $person_id");
      if (!count($recs)||$recs[0]["cnt"]==0)
      {
        $db->query("INSERT INTO project_person (projectid, personid, role) VALUES ($project_id, $person_id, $role_id)");
      }
    }

    /**
     * Are we in 'active' emps or 'archive' mode?
     */
    function getView()
    {
      global $g_sessionManager;
      $view = $g_sessionManager->stackVar("view");
      if ($view=="")
      {
        $view = "active";
      }
      return $view;
    }

    /**
     * Action select, add filter if user doens't have
     * the any project right.
     *
     * @param object $handler Handler object
     * @return string Select page
     */
    function action_select(&$handler)
    {
      if (!$this->allowed("any_project"))
      {
        $pids = implode(",", $this->get_user_projects());
        if(!empty($pids))
          $this->addFilter("project.id IN ($pids) OR project.timereg_limit = ".PRJ_TIMEREG_ALL_USERS);
        else
          $this->addFilter("project.timereg_limit = ".PRJ_TIMEREG_ALL_USERS);
      }
      return $handler->action_select();
    }

    /**
     * Get user project id's
     *
     * @return array Array with project id's
     */
    function get_user_projects()
    {
      $db = &atkGetDb();
      $sql = "SELECT DISTINCT projectid
                 FROM project_person
                 WHERE personid = '".atkGetUserId()."'";
      $records = $db->getrows($sql);
      $id_array = array();
      foreach($records as $row)
      {
        array_push($id_array, $row["projectid"]);
      }
      array_push($id_array, 0);
      return $id_array;
    }

    function action_admin(&$handler, $record=null)
    {
      $view = $this->getView();
      if ($view=="active")
      {
        $this->addFilter("project.status","active");
      }
      else
      {
        $this->addFilter("project.status<>'active'");
      }


      // Access control
      if (!$this->allowed("any_project"))
      {
        $user = atkGetUser();
        $pids = implode(",", $this->get_user_projects());

        $this->addFilter("project.id IN ($pids) OR project.coordinator=".$user["id"]);

        // we can hide the coordinator column from the list in this case.
        //$attr = &$this->getAttribute("coordinator");
        //$attr->addFlag(AF_HIDE_LIST);
      }

      return $handler->action_admin($record);
    }

    /**
     * Edit action handler override
     *
     * Sets the Abbreviation attribute to readonly if the user doesn't have the changeabbreviation right
     *
     * @param atkEditHandler $handler
     * @return string HTML
     */
    function action_edit(&$handler)
    {
      if (!$this->allowed("changeabbreviation"))
      {
        $abbrattr = &$this->getAttribute("abbreviation");
        $abbrattr->addFlag(AF_READONLY);
      }

      $handler->addRenderBoxVar('projectmenu', $this->getProjectMenu());

      return $handler->action_edit();
    }

    /**
     * View action handler override
     *
     * Add an extra param to the template
     *
     * @param atkViewHandler $handler
     * @return string HTML
     */
    function action_view(&$handler)
    {
      $handler->addRenderBoxVar('projectmenu', $this->getProjectMenu());
      return $handler->action_view();
    }

    /**
     * Overrideable function to create a header for view mode.
     * Similar to the admin header functionality.
     */
    function viewHeader()
    {
      return $this->getProjectMenu();
    }

    function getProjectMenu()
    {
      atkimport('module.project.projectmenu');
      $pm = new projectmenu('project.project', $this->project);
      return $pm->getTabHtml();
    }

    function adminFooter()
    {
      $view = $this->getView();

      if ($view=="active")
      {
        return atktext("onlyactiverecordsareshown")." ".href(dispatch_url($this->atknodetype(),$this->m_action,array("view"=>"nonactive")),
                                                        atktext('clicktoviewarchive', $this->m_module, $this->m_type))."<br>";
      }
      else
      {
        return atktext("youareinthearchive")." ".href(dispatch_url($this->atknodetype(),$this->m_action,array("view"=>"active")),
                                                     atktext('clicktoviewactiverecords', $this->m_module, $this->m_type))."<br>";
      }
    }

    /**
     * Function removes double selected templates (which happens when
     * using the projecttemplate and selecting additional phasetemplates.
     *
     * @param array() $record description
     * @return boolean succeeded
     */
    function preAdd(&$record)
    {
      // Add enddate to an additional field
      $record['initial_enddate'] = $record['enddate'];

      //get all tpl_project_phase combinations with the selected
      //projecttemplate id.
      $tpl_project_phase_Node = &atkGetNode("project.tpl_project_phase");
      $selector = sprintf("projectid='%s'",$record["template"]["id"]);
      $phase_tpls = $tpl_project_phase_Node->selectDb($selector);

      //retrieve all the tpl_phase_ids
      $ids = array();
      foreach($phase_tpls as $tpl)
        $ids[] = $tpl["phaseid"]["id"];

      //Add the unique ones in a new array.
      $newmatrix = array();
      for($i=0,$_i=count($record["phasetemplatematrix"]);$i<$_i;$i++)
      {
        $matrix_tpl_id = $record["phasetemplatematrix"][$i]["template"];

        if(!in_array($matrix_tpl_id,$ids))
          $newmatrix[] = $record["phasetemplatematrix"][$i];
      }
      //replace the old array with a the new (filtered) array.
      $record["phasetemplatematrix"] = $newmatrix;

      return true;
    }

    function contract_id_edit($record, $mode)
    {
      $fromdate = $this->m_attribList["startdate"]->value2db($record);

      $filter = "(contract.status='active'";
      if ($fromdate!="NULL")
      {
        $filter.= " AND '$fromdate' BETWEEN contract.startdate AND contract.enddate";
      }
      $filter.=") ";
      if ($record["contract_id"]["id"]!="")
      {
        // if a current record is selected, we must include that record, even if it falls outside the filter
        $filter.= " OR contract.id=".$record["contract_id"]["id"];
      }

      $this->m_attribList["contract_id"]->m_destinationFilter = $filter;

      return $this->m_attribList["contract_id"]->edit($record, $mode);
    }

    function postUpdate($record)
    {
      $this->addCoordinatorToTeam($record);
    	$this->updatePhaseFields($record, "update");
      return true;
    }

    /**
    * This function updates the templates that are added using
    * the atkManyBoolRelation.
    *
    * Function copies the name field from the phase-templates to
    * the corresponding phases.
    *
    * @todo This code belongs logically in the phase node.
    * @param array $record the added record.
    * @return type description
    */
    function updatePhaseFields($record, $mode)
    {
     $phaseTplNode = &atkGetNode("project.tpl_phase");
     $phaseNode = &atkGetNode("project.phase");
/*
     foreach($record["phasetemplatematrix"] as $tpl)
     {
       atkdebug("Using template ".$tpl["template"]);
       //get the template
       $template_id = $tpl["template"];
       $selector = sprintf("tpl_phase.id='%s'",$template_id);
       $template = $phaseTplNode->selectDb($selector);

       //get the corresponding phases for this project
       $selector = sprintf("phase.template='%s' AND phase.projectid='%s'",$template_id,$record["id"]);
       $phases = $phaseNode->selectDb($selector);

       //update each phase with the name and description fields.
       //and add the activities
       foreach($phases as $phase)
       {
         $template_id = $phase["template"]["id"];
         $selector = "tpl_phase.id='$template_id'";
         $template = $phaseTplNode->selectDb($selector);

         $newphase["id"]           = $phase["id"];
         $newphase["atkprimkey"]   = $phase["atkprimkey"];
         $newphase["status"]       = 'active';

         //if we are updating a project, we cannot overwrite existing phase data
         if($mode != "update")
         {
           atkdebug("updating name and description fields");
           $newphase["name"]         = $template[0]["name"];
           $newphase["description"]  = $template[0]["description"];
           $phaseNode->updateDb($newphase,false,"",array("name","description", "status"));
         }
         $this->addActivitiesPerPhase($template_id, $newphase["id"]);
       }
     } */
    }

    function addActivitiesPerPhase($tpl_phase_id, $phaseid)
    {
      atkdebug("Adding activities per phase");

      //Add activities per phase (if they do not exist yet).
      $node = &atkGetNode("project.phase_activity");
      /* @var $node atkNode */
      $selector = "phaseid='$phaseid'";
      $records = $node->selectDb($selector,"","","",array("activityid"));

      $ids = array();
      foreach($records as $rec)
        $ids[] = $rec["activityid"]["id"];

      $and_where = count($ids) ? " AND activityid NOT IN (".implode(",",$ids).")" : "";

      //activities per phase
      $db = &atkGetDb();
      $query = "INSERT INTO phase_activity (activityid, phaseid)
                SELECT activityid, ".$phaseid."
                FROM tpl_phase_activity
                WHERE phaseid = '".$tpl_phase_id."'".$and_where;

      $db->query($query);
    }

    function _hideMasterProject($id)
    {
      if(atkConfig::get("project","use_subprojects"))
      {
        $rows = &$this->selectDb("project.master_project='$id'","","","",array('id'));

        if(count($rows)>0)
        {
          $p_attr = &$this->getAttribute("master_project");
          $p_attr->addFlag(AF_HIDE);
        }
      }
    }

    public function editPage(&$handler, $record, $locked=FALSE)
    {
      $this->_hideMasterProject($record['id']);
      return $handler->editPage($record, $locked);
    }

    public function viewPage(&$handler, $record, $locked=FALSE)
    {
      $this->_hideMasterProject($record['id']);
      return $handler->viewPage($record, $locked);
    }

    /**
     * Template override for custom traffic light display
     *
     * @param String $action
     * @param array $record
     * @param String $tab
     * @return String The filename of the template (without path)
     */
    public function getTemplate($action, $record=NULL, $tab="")
    {
      switch ($action)
      {
        case "add": return "editform.tpl";
        case "edit": return "editform_trafficlight.tpl";
        case "view": return "viewform_trafficlight.tpl";
      }
      return parent::getTemplate($action, $record, $tab);
    }

   /**
    * Set the project id.
    */
    private function setProjectId()
    {
      $vars = atkGetPostVar();
      if ($vars['atknodetype'] == 'project.project' && array_key_exists('atkselector', $vars) && $vars['atkselector'] != '')
      {
        $this->project = $vars['atkselector'];
        sessionStore('projectid', $vars['atkselector']);
      }
      else
      {
        $this->project = sessionLoad('projectid');
      }
    }
    
    /**
     * Add coordinator from current record to team
     *
     * @param array $rec - current project record 
     */
    private function addCoordinatorToTeam($rec)
    {
    	if($rec['coordinator']['id'])
    	{
    		$role_id = atkGetNode("project.role")->getCoordinatorId();
    		$this->_addMemberRecord($rec["id"], $rec['coordinator']['id'], $role_id);
    		//delete old coordinator from team
    		$this->getDb()->query("
    			DELETE FROM project_person 
    			WHERE
    				projectid='".$rec["id"]."' AND 
    				personid!='".$rec['coordinator']['id']."' AND
    				role='".$role_id."'
    		");
    	}
    }

  }