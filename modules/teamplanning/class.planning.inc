<?php

  useattrib("atkattribute");
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  //useattrib("teamplanning.planningprojectphaseattribute");
  userelation("atkmanytoonerelation");

  atkimport("module.utils.dateutil");

  require_once(moduleDir("teamplanning")."utils/class.teamplanningrenderer.inc");
  require_once(moduleDir("teamplanning")."utils/class.teamitem.inc");
  require_once(moduleDir("teamplanning")."utils/class.projectitem.inc");
  require_once(moduleDir("teamplanning")."utils/class.phaseitem.inc");
  require_once(moduleDir("teamplanning")."class.teamcapacity.inc");
  require_once(moduleDir("teamplanning")."class.workload.inc");
  
 
  class planning extends atkNode
  {
    function planning($type = "planning", $flags = 0)
    {
      $this->atkNode($type, $flags);
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      //the provided date is the date of the first day of the week.
      $this->add(new atkDateAttribute("date", AF_OBLIGATORY));
      $this->add(new atkManyToOneRelation("teamid", "employee.team", AF_OBLIGATORY));
      //we support project and phase. In the future we may also support activities
      $this->add(new atkManyToOneRelation("projectid", "project.project", AF_OBLIGATORY));
      $this->add(new atkManyToOneRelation("phaseid", "project.phase", AF_OBLIGATORY));
      $this->add(new atkNumberAttribute("time", AF_OBLIGATORY));
      //@todo: add configuration to force buffer on projectlevel.
      //we support buffers on both project and phaselevel. With a configuration we can
      //force the use of the buffer on projectlevel.
      $this->add(new atkNumberAttribute("buffertime", AF_OBLIGATORY));
      $this->setTable("teamplanning_planning");
    }
    
    /**
     * Update handler function
     */
    function action_update()
    {
      atkimport("module.planning.conflictlistener");
      $this->addListener(new conflictListener(array("update", "save")));
      
      // Read the posted values
      $data = $this->m_postvars["data"];
      $olddata = $this->m_postvars["olddata"];

      // Get a reference to the database object
      $db = &$this->getDb();
      
      $analysis_queue = array();

      // Loop through all cells
      foreach($data as $cell => $value)
      {

        // If the value is different from the original (old) value, then start update this particular planning value
        if ($value != $olddata[$cell]) {

          // Determine the employee_id, monday and phase_id variable according to the cellname
          list($teamid, $date, $projectid, $phaseid) = explode("_", $cell);          
          
          //$db->query("DELETE FROM weekplanning_planning WHERE employeeid='".$employee_id."' AND monday='".$monday."' AND phaseid='".$phase_id."'");

          // Store the given planningvalue if set and higher than 0
          if ($value > 0)
          {
//            atkdebug("This module is ".$this->m_module);
//            $id = $db->nextid("planning");
//            $db->query("INSERT INTO weekplanning_planning (id, employeeid, monday, phaseid, minutes)
//                               VALUES ($id, $employee_id, '$monday', $phase_id, ".($value*60).")");
//            $analysis_queue[] = array("phaseid"=>$phase_id, "employeeid"=>$employee_id, "monday"=>$monday);
//            atkdebug("This module is now ".$this->m_module);
          }
        }              

      }
      
      // After new situation is stored, perform all notifications at once, so we analyse conflicts
      // only for the complete situation.
      foreach ($analysis_queue as $item)
      {
        //$this->notify("update", $item);
      }

      // Redirect to the user to the admin page
      $this->redirect();
    }

    /**
     * Return record descriptor (for use in relations etc.)
     *
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      // @todo: replace this with proper descriptor fields.
      return "[id]";
    }

    /**
     * Generic admin actionhandler for the weekplanning module
     *
     * This function implements the call to the specific adminPage functio
     * which is to be implemented in any classes that derive from this
     * class. the adminPage function has a simple interface; it takes no
     * parameters:
     *
     *   function adminPage()
     *
     * if the adminPage is not implemented or the admin action is directly
     * performed on an instance of this class, then an atkerror is thronwn.
     *
     * @param atkAdminHandler Reference to the default atkActionHandler for the admin action (note: this parameter is of no use and may be omitted as the recordlistbehaviour of the default atkAdminHandler isn't what we want)
     */
    function action_admin($handler = null)
    {
      // If adminPage implemented, call it and store the content result.
      $content = $this->adminPage();

      // Add the given content to the page
      $page = &$this->getPage();
      $page->addContent($this->renderActionPage("admin", $content));
    }

    /**
     * Generates the HTML content to be displayed when using the admin function
     *
     * @return string HTML Content to be displayed
     */
    function adminPage()
    {
      // Get a reference to the sessionmanager
      global $g_sessionManager;

      // Use the generic style
      $this->addStyle("style.css");       //add base style
      $this->addStyle("recordlist.css");  //add style for recordslists
      $page = &$this->getPage();
      $page->register_script(moduleDir($this->m_module)."javascript/planning.js");

      // Try to get a reference to the User Interface object and raise an error if it fails
      $ui = &$this->getUi();
      if (!is_object($ui))
      {
        atkerror("Failed to create ui object");
        return "";
      }

      $go = $g_sessionManager->pageVar("go");

      // Start composing the output by adding the filterbar as topmost item
      if (!$this->isPrinter())
      {
        $output = $this->getFilterBar();
        if ($go==1)
        {
          $output.= $this->getDateNavBar();
        }
      }

      // Add a newline (vertical spacing)
      $output.= '<br/>';

      // Only get the planning if the filter was set and submitted
      if ($go == 1)
      {
        $output .= $this->getPlanning().'<br/>';
      }

      // Render a box around the generated output
      $boxedoutput = $ui->renderBox(array("title"=>$this->actionTitle('admin'), "content"=>$output));

      // Return the generated boxed output
      return $boxedoutput;
    }

    function isPrinter()
    {
      return ($this->m_postvars["mode"]=="print");
    }

    /**
     * Composes relative date-shifted navigation links
     *
     * Composes a date navigation link used to increase or decrease the
     * starting or ending date by a fixed amount depending on the current
     * filterrecord.
     *
     * @param array $record Current filter record
     * @param integer $sd Shift factor for the starting day
     * @param integer $sm Shift factor for the starting month
     * @param integer $sy Shift factor for the starting year
     * @param integer $ed Shift factor for the ending day
     * @param integer $em Shift factor for the ending month
     * @param integer $ey Shift factor for the ending year
     * @param String $linktext Text to show as link
     * @param String $translationmodule Module from which the translation of given linktext can be retrieved
     * @return String HTML Link plus hard space (&nbsp;)
     */
    function getDateNavLink($record, $sd, $sm, $sy, $ed, $em, $ey, $linktext, $translationmodule = "")
    {
      $nmf = dateutil::stamp2arr(mktime(12,0,0,$record["from"]["month"]+$sm, $record["from"]["day"]+$sd, $record["from"]["year"]+$sy));
      $nmt = dateutil::stamp2arr(mktime(12,0,0,$record["to"]["month"]+$em, $record["to"]["day"]+$ed, $record["to"]["year"]+$ey));

      return href("?from[year]=".$nmf["year"]."&from[month]=".$nmf["month"]."&from[day]=".$nmf["day"].
                     "&to[year]=".$nmt["year"]."&to[month]=".$nmt["month"]."&to[day]=".$nmt["day"] , atktext($linktext, $translationmodule))."&nbsp;";
    }

    /**
     * Composes a bar containing links to navigate through the weekplanning by jumping fixed amounts of time back and forward
     *
     * @return String HTML Navigation bar
     */
    function getDateNavBar()
    {
      $record = $this->getFilterRecord();
      $output = $this->getDateNavLink($record, -7, 0, 0, -7, 0, 0, "previousweek");
      $output.= $this->getDateNavLink($record, 0, -1, 0, 0, -1, 0, "previous_month");
      $output.= $this->getDateNavLink($record, 0, 1, 0, 0, 1, 0, "next_month");
      $output.= $this->getDateNavLink($record, 7, 0, 0, 7, 0, 0, "nextweek");
      $output.= $this->getDateNavLink($record, 0, 0, 0, 7, 0, 0, "add_week", "weekplanning");
      $output.= $this->getDateNavLink($record, 0, 0, 0, -7, 0, 0, "remove_week", "weekplanning");
      $output.= href("?mode=print", atktext("report_output_printable"));
      return $output."<br/>";
    }

    /**
     * Formats a week identifier so it can be displayed in a more friendly format
     *
     * @param String $yyyyww Week identifier in yyyyww format
     * @return String Week display value in yyyy-ww format
     */
    function weekFormat($yyyyww)
    {
      return substr($yyyyww, 0, 4)."-".substr($yyyyww, 4);
    }

    /**
     * Calculates the weeknumber, startdate/enddate of the week for each week within the posted from and to values
     *
     * @return array Array containing arrays like "week"=>weeknumber,"startdate"=>startdate,"enddate"=>enddate
     */
    function getWeeksBetween()
    {
      $filterrecord = $this->getFilterRecord();
      // Initialize the weekdata array
      return dateUtil::weeksBetween(dateUtil::arr2str($filterrecord["from"]), 
                                       dateUtil::arr2str($filterrecord["to"]));  
    }

    function hasAnyUserPrivilege()
    {
      return $this->allowed("any_user");
    }

    function hasAnyProjectPrivilege()
    {
      return $this->allowed("any_project");
    }

    function getFilterParams()
    {
      $record = $this->getFilterRecord();
      return array("from[year]"=>$record["from"]["year"],
                   "from[month]"=>$record["from"]["month"],
                   "from[day]"=>$record["from"]["day"],
                   "to[year]"=>$record["to"]["year"],
                   "to[month]"=>$record["to"]["month"],
                   "to[day]"=>$record["to"]["day"],
                   "go"=>1);
    }

    function getConflicts()
    {
      $output = "<b>".atktext("conflicts")."</b>:<br/><br/>";

      // List all present conflicts.
      $cn = &getNode("planning.planningconflictlog");
      $conflicts = $cn->selectDb();

      $rl = &atknew("atk.recordlist.atkrecordlist");

      $output.= $rl->render($cn, $conflicts, array(), RL_NO_SEARCH|RL_NO_SORT);

      return $output;
    }
        
    function getFilterRecord()
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if ($s_record==NULL)
      {
        $s_record = array();        
        $s_record["supervisor"] = $g_sessionManager->pageVar("supervisor");
        $s_record["coordinator"] = $g_sessionManager->pageVar("coordinator");
        $s_record["from"] = $g_sessionManager->pageVar("from");
        $s_record["to"] = $g_sessionManager->pageVar("to");

        // convert attribute html values to internal values
        $attrs = &$this->getFilterAttrs();
        foreach (array_keys($attrs) as $attribname)
        {
          $p_attrib = &$attrs[$attribname];
          $s_record[$attribname] = &$p_attrib->fetchValue($s_record);
        }


        $go = $g_sessionManager->pageVar("go");
        if ($go!=1)
        {
          // initial loading. We'll put the from/to fields to reasonable defaults.

          $s_record["from"] = dateUtil::str2arr(dateUtil::startOfWeek(date("Ymd")));

          // next month
          $next_month = mktime(12,0,0,date("m")+1, date("d"), date("Y"));
          $s_record["to"] = dateUtil::str2arr(dateUtil::endOfWeek(date("Ymd", $next_month)));
        }
      }
      return $s_record;
    }

    function &getFilterAttrs()
    {
      $attrs = array();

      useattrib("employee.myemployeesattribute");

      $attrs["supervisor"]  = &new myEmployeesAttribute("supervisor", "teamplanning.planning.any_user");
      $attrs["coordinator"] = &new myEmployeesAttribute("coordinator", "teamplanning.planning.any_project");

      $attrs["from"]        = &new atkDateAttribute("from");
      $attrs["to"]          = &new atkDateAttribute("to");

      return $attrs;
    }

    function getFilterBar()
    {
      $record = $this->getFilterRecord();

      $attrs = &$this->getFilterAttrs();

      $output = '<form action="dispatch.php" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time

      if ($this->hasAnyUserPrivilege())
      {
        $output.= atktext("supervisor").": ".$attrs["supervisor"]->edit($record)." ";
      }
      if ($this->hasAnyProjectPrivilege())
      {
        $output.= atktext("coordinator").": ".$attrs["coordinator"]->edit($record)." ";
      }

      $output.= "<br/>";

      $output.= atktext("period_from", "weekplanning")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("to", "weekplanning")." ".$attrs["to"]->edit($record);

      $output.= ' <input type="submit" value="'.atktext("refresh").'">';

      $output.= '</form>';
      return $output;
    }

    function getPlanning()
    {
      $result = "";

      $filterrecord = $this->getFilterRecord();
      $fromdate     = $filterrecord["from"]["year"]."-".$filterrecord["from"]["month"]."-".$filterrecord["from"]["day"];
      $todate       = $filterrecord["to"]["year"]."-".$filterrecord["to"]["month"]."-".$filterrecord["to"]["day"];
      $period       = array("from"=>$fromdate, "to"=>$todate, "weeks"=>$this->getWeeksBetween());
      $teamscapacityobj = &new teamcapacity();
      $workloadobject   = &new workload();
      $teamsdata    = $teamscapacityobj->getCapacity($period);
      $workload     = $workloadobject->getWorkLoad();      
      $planneddata  = $this->getPlannedData($period);
      
      //we need to combine the data we found to use it in the planning.
      
      return $this->renderPlanning($teamsdata,$workload,$period,$planneddata);
    }   
    
    function renderPlanning(&$teamsdata,&$workload,&$period,&$planneddata)
    { 
      $planning = &new teamPlanningRenderer();
      
      $result = '<form name="entryform" action="dispatch.php" method="post">';
      $result.= session_form();
      $result.= '<input type="hidden" name="atkaction" value="update">';
      
      //header fields
      $headerfields = array("name","initial_planning","current_planning","current_min_planned");      
      $weeknames    = array();
      foreach($period["weeks"] as $week)  //get weeknames      
        $weeknames[] = $week["week"];

      atk_var_dump($workload,"<h1>HAR TAH WORKLOAD</h1>");
      $planning->setHeader($headerfields,$weeknames);
      foreach($teamsdata as $team)
      {
        $teamkey = $team["id"];
        
        $teamItem = &new teamItem($teamkey);
        $teamItem->setTitle("Team: ".$team["name"]);
        $teamItem->setMaxCapacity($team);
        
        //render workload
        foreach($workload as $project)
        {
          $projectkey = $project["id"];
          $projectItem = &new projectItem($projectkey, $teamkey);
          $projectItem->setHeaderData($project);
          $projectItem->setWeekData($planneddata[$teamkey]["projects"][$projectkey]["planneddata"]);          
          $projectItem->setConflicts(array("OH NO A CONFLICT!"));
          foreach($project["phases"] as $phase)
          {
            $phasekey = $phase["id"];
            $phaseItem = &new phaseItem($phasekey,$teamkey,$projectkey);
            $phaseItem->setHeaderData($phase);            
            $phaseItem->setWeekData($planneddata[$teamkey]["projects"][$projectkey]["phases"][$phasekey]["planneddata"]);
            $projectItem->addItem($phaseItem);  
          }
          $teamItem->addItem($projectItem);
        }        
        $planning->addItem($teamItem);
      }      
      $result.= $planning->render();
      
      $result.= '<br /><input type="submit" value="'.atktext("save").'"> ';

      $result.= '</form>';
      return $result;
    }
    
    
    /**
     * Get all planned items that fall within the given period.     
     *
     * @param type name description
     * @return type description
     */
    function getPlannedData($period)
    { 
      /* @var $query atkQuery */
      $db     = &atkGetDb();
      $query  = &new atkQuery();
                 
      $fromdate = $period["from"];
      $todate   = $period["to"];
      
      $query->addField("id","","teamplanning_planning","",true);
      $query->addField("date","","teamplanning_planning","",true);
      $query->addField("teamid","","teamplanning_planning","",true);
      $query->addField("projectid","","teamplanning_planning","",true);
      $query->addField("phaseid","","teamplanning_planning","",true);
      $query->addField("time","","teamplanning_planning","",true);
      $query->addField("buffertime","","teamplanning_planning","",true);
      $query->addTable("teamplanning_planning");
      $query->addCondition("'$fromdate' <= teamplanning_planning.date AND '$todate' >= teamplanning_planning.date");
      $records = $db->getrows($query->buildSelect());      

      $planneddata = array();
      //reformat
      foreach($records as $rec)
      {
        //$id         = $rec["id"];
        $teamid     = $rec["teamid"];
        //convert date to a weekstamp.
        $week       = weekstamp(adodb_mktime(12,0,0,substr($rec["date"],5,2),substr($rec["date"],8,2),substr($rec["date"],0,4)));        
        $projectid  = $rec["projectid"];
        $phaseid    = $rec["phaseid"];
        $time       = $rec["time"];
        $buffertime = $rec["buffertime"];
        
        //@todo: add config to force the use of only buffertimes for projects and then ignore the
        //found buffertimes for phases. For now, at default we ignore the buffertimes for phases.
        
        //if the phaseid is 0, the time and buffer belong to a project registration.
        if($phaseid == 0)
        {
          $planneddata[$teamid]["projects"][$projectid]["planneddata"][$week]["time"] = $time;
          $planneddata[$teamid]["projects"][$projectid]["planneddata"][$week]["buffertime"] = $buffertime;
        }
        else 
        {
          $planneddata[$teamid]["projects"][$projectid]["phases"][$phaseid]["planneddata"][$week]["time"] = $time;
          //$planneddata[$teamid]["projects"][$projectid]["phases"][$phaseid][$week]["buffertime"] = $buffertime;
        }
      }
      
      atk_var_dump($planneddata,"<h1>PLANNEDDATA</h1>");

      return $planneddata;
    }
    
    
    
  }
?>