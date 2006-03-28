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
  require_once(moduleDir("teamplanning")."utils/class.activityitem.inc");
  require_once(moduleDir("teamplanning")."class.teamcapacity.inc");
  require_once(moduleDir("teamplanning")."class.workload.inc");
  
 
  class teamplanning extends atkNode
  {
    var $m_dataprefix     = "newdata_";
    var $m_olddataprefix  = "olddata_";
    
    function teamplanning($type = "teamplanning", $flags = 0)
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
      //$this->addListener(new conflictListener(array("update", "save")));
            
      // Read the posted values (old, new and datearr)      
      $data     = array();  $olddata  = array();  $datearr = array();
      $this->getPostedData($data, $olddata, $datearr);    //we add two arrays by value to store the found items
           
      $changeddata = array();

      // Loop through all cells
      foreach($data as $cell => $value)
      {
        // If the value is different from the original (old) value, then start update this particular planning value
        if ($value != $olddata[$cell]) 
        {
          /*          
            Determine the values, according to the cellname
            
            The cellname contains 6 parts:
            teamid_projectid_phaseid_isbuffer_weekid            
            
            teamid:     contains the id of the team
            projectid:  contains the id of the project
            phaseid:    contains the id of the phase or is empty, if the value belongs only to a project
            buffer:     contains "0" if the value is not a buffertime, and contains "buffer" if the value
                        IS a buffertime.
            weekid:     The weekid
          */          
          list($teamid, $projectid, $phaseid, $buffer, $weekid) = explode("_", $cell);
                  
          $value = ($value>=0 && $value!="") ? $value : 0;
                    
          //what kind of change is made? 
          //possibilities: project time, project-buffer time, phase time or phase-buffer time?          
          if($phaseid == 0)
          {
            if($buffer != 'buffer')
            {
              atkdebug("project item, no buffer");
              $changeddata[$teamid][$projectid]["changes"][$weekid]["time"] = $value;
            }
            else 
            {
              atkdebug("project item, buffer");
              $changeddata[$teamid][$projectid]["changes"][$weekid]["buffertime"] = $value;
            }
          }
          else 
          {
            if($buffer != 'buffer')
            {
              atkdebug("phase item, no buffer");
              $changeddata[$teamid][$projectid]["phases"][$phaseid]["changes"][$weekid]["time"] = $value;
            }
            else
            {
              atkdebug("phase item, buffer");
              $changeddata[$teamid][$projectid]["phases"][$phaseid]["changes"][$weekid]["buffertime"] = $value;
            }
          }
        }
      }
      
      $analysis_queue = array();
      
      //work with the changed data
      foreach($changeddata as $teamid=>$projects)
      {        
        foreach($projects as $projectid=>$project)
        {
          //project changes per week
          $this->_loopChangedItems($teamid, $projectid, 0, $project, $datearr, $analysis_queue);
          
          //any phases changed?
          if(isset($project["phases"]))
          {
            foreach($project["phases"] as $phaseid=>$phase)
            {
              //phase changes per week
              $this->_loopChangedItems($teamid, $projectid, $phaseid, $phase, $datearr, $analysis_queue);
            }
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
    
    function _loopChangedItems($teamid, $projectid, $phaseid, $items, &$datearr, &$analysis_queue)
    {
      //atk_var_dump($items,"<h1>items</h1>");
      
      if(!isset($items["changes"])) 
        $items["changes"] = array();
      
      foreach($items["changes"] as $weekid=>$change)
      {
        $date = "";
        if(isset($datearr[$weekid]))        
          $date = $datearr[$weekid];
        else 
        {
          //we cannot do anything without a proper date
          atkdebug("no date found, ignoring this planning item.");
          break;
        }
        
        $selector = "teamplanning_planning.teamid='$teamid' AND teamplanning_planning.projectid='$projectid' AND teamplanning_planning.phaseid='$phaseid' AND teamplanning_planning.date='$date'";
        $records = $this->selectDb($selector);
        
        $numrecs = count($records);        
        /*
          if we have a record
        */
        if($numrecs > 0)
        { 
          //do we need to delete this record?
          if($this->_recordMustBeDeleted($records[0],$change["time"],$change["buffertime"]))
          {
            //delete
            atkdebug("deleting".$records[0]["atkprimkey"]);
            $analysis_queue[] = array("projectid"=>$projectid,"phaseid"=>$phaseid, "teamid"=>$employee_id, "date"=>$date);
            $this->deleteDb($records[0]["atkprimkey"],false);
          }
          else 
          {
            //update            
            if(isset($change["time"]) && $change["time"]!="")            
              $records[0]["time"]       = $change["time"]*60;
            if(isset($change["buffertime"]) && $change["buffertime"]!="")
              $records[0]["buffertime"] =  $change["buffertime"]*60;
            atkdebug("updating id=".$records[0]["id"]);
            $analysis_queue[] = array("projectid"=>$projectid,"phaseid"=>$phaseid, "teamid"=>$employee_id, "date"=>$date);
            $this->updateDb($records[0],false);            
          }
        }
        //no records found.
        else 
        {
          //do we need to add a new record?
          if($this->_recordMustBeAdded($change["time"],$change["buffertime"]))
          {
            $newrec = array();
            $newrec["projectid"]  = array("id"=>$projectid);
            $newrec["phaseid"]    = array("id"=>$phaseid);
            $newrec["teamid"]     = array("id"=>$teamid);
            $newrec["date"]       = $date;
            $newrec["time"]       = (isset($change["time"]) && $change["time"]!="") ? $change["time"]*60 : 0;
            $newrec["buffertime"] = (isset($change["buffertime"]) && $change["buffertime"]!="") ? $change["buffertime"]*60 : 0;
            atkdebug("adding new record");
            $analysis_queue[] = array("projectid"=>$projectid,"phaseid"=>$phaseid, "teamid"=>$employee_id, "date"=>$date);
            $this->addDb($newrec,false);
          }
          else 
          {
            //do nothing
          }
        }            
      }
    }
    
    function _recordMustBeAdded($newtime,$newbuffertime)
    {
      /*
        If both the time and buffertime have been changed to 0, we do not need to update        
      */
      if(isset($change["time"]) && $change["time"]==0 && isset($change["buffertime"]) && $change["buffertime"]==0)
      {
        return false;
      }
      /*
        if only time has been set to 0 and buffertime has not been touched, we do not need to update
      */
      if(isset($change["time"]) && $change["time"]==0 && !isset($change["buffertime"]))
      {
        return false;
      }
      /*
        if only buffertime has been set to 0 and time has not been touched, we do not need to update
      */
      if(isset($change["time"]) && $change["time"]==0 && !isset($change["buffertime"]))
      {
        return false;
      }
      
      //we need to add one
      return true;
    }
    
    function _recordMustBeDeleted($record, $newtime, $newbuffertime)
    {
      /*  
        if both the time and buffertime have been changed to 0 we remove the record.
      */
      if(isset($newtime) && $newtime==0 && isset($newbuffertime) && $newbuffertime==0)
      {
        return true;
      }      
      
      /*
        if only time has been set to 0 and buffertime has not been touched.
      */
      if(isset($newtime) && $newtime==0 && !isset($newbuffertime))
      {
        /*  
          Check if the buffertime is already 0 in the database record. If so, delete the record.
        */
        if(!isset($record["buffertime"]) || $records["buffertime"]==0 || $records["buffertime"]=="")
        {
          return true;
        }
        /*
          If the buffertime is not 0, we update the record
        */
        else
        {
          return false;
        }
      }
      
      /*
        if only buffertime has been set to 0 and time has not been touched.
      */
      if (isset($newbuffertime) && $newbuffertime==0 && !isset($newtime))
      {
        /*  
          Check if the time is already 0 in the database record. If so, delete the record.
        */
        if(!isset($records["time"]) || $records["time"]==0 || $records["time"]=="")
        {
          return true;
        }
        /*
          If the time is not 0, we update the record
        */
        else
        {
          return false;
        }
      }
      
      return false;
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
      $output.= $this->getDateNavLink($record, 0, 0, 0, 7, 0, 0, "add_week", "teamplanning");
      $output.= $this->getDateNavLink($record, 0, 0, 0, -7, 0, 0, "remove_week", "teamplanning");
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
      // List all present conflicts.
      $cn = &getNode("teamplanning.teamplanningconflictlog");
      $conflicts = $cn->selectDb();

      //@todo: reformat the conflicts
      
      return $conflicts;
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

      $attrs["supervisor"]  = &new myEmployeesAttribute("supervisor", "teamplanning.teamplanning.any_user");
      $attrs["coordinator"] = &new myEmployeesAttribute("coordinator", "teamplanning.teamplanning.any_project");

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

      $output.= atktext("period_from", "teamplanning")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("to", "teamplanning")." ".$attrs["to"]->edit($record);

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
      $planneddata  = $this->getPlannedData();
      $conflicts    = $this->getConflicts();
      
      //we need to combine the data we found to use it in the planning.
      
      return $this->renderPlanning($teamsdata,$workload,$period,$planneddata);
    }   
    
    function renderPlanning(&$teamsdata,&$workload,&$period,&$planneddata)
    { 
      $planning = &new teamPlanningRenderer($this->m_dataprefix,$this->m_olddataprefix);
      
      $result = '<form name="entryform" action="dispatch.php" method="post">';
      $result.= session_form();
      $result.= '<input type="hidden" name="atkaction" value="update">';
      
      //header fields
      $headerfields = array("name","initial_planning","current_planning","current_min_planned");      

      $planning->setHeader($headerfields,$period["weeks"]);      
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
            foreach($phase["activities"] as $activity)
            {
              $activitykey = $activity["id"];
              $activityItem = &new activityItem($activitykey,$teamkey,$projectkey,$phasekey);
              $activityItem->setHeaderData($activity);
              $activityItem->setWeekData($planneddata[$teamkey]["projects"][$projectkey]["phases"][$phasekey]["activities"]["planneddata"]);
              $phaseItem->addItem($activityItem);
            }
            $projectItem->addItem($phaseItem);  
          }
          $teamItem->addItem($projectItem);
        }        
        $planning->addItem($teamItem);
      }
      $result.= $planning->render();
      
      $result.= '<br /><input type="submit" value="'.atktext("save").'">';

      $result.= '</form>';
      return $result;
    }
    
    
    /**
     * Get all planned items.
     *
     * @param type name description
     * @return type description
     */
    function getPlannedData()
    { 
      /* @var $query atkQuery */
      $db     = &atkGetDb();
      $query  = &new atkQuery();

      $query->addField("id","","teamplanning_planning","",true);
      $query->addField("date","","teamplanning_planning","",true);
      $query->addField("teamid","","teamplanning_planning","",true);
      $query->addField("projectid","","teamplanning_planning","",true);
      $query->addField("phaseid","","teamplanning_planning","",true);
      $query->addField("time","","teamplanning_planning","",true);
      $query->addField("buffertime","","teamplanning_planning","",true);
      $query->addTable("teamplanning_planning");      
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
        //found buffertimes for phases. For now, at default we ignore the buffertime for phases.
        
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
    
    function getPostedData(&$data, &$olddata, &$datearr)  
    {
      $pre            = $this->m_dataprefix;
      $oldpre         = $this->m_olddataprefix;
      $pre_len        = strlen($pre);      
      $oldpre_len     = strlen($oldpre);
      $weekdates_len  = strlen("weekdate_");
      
      //we loop through the posted data, and get the data, and olddata
      //values, we remove the prefixes.
      foreach($this->m_postvars as $itemkey=>$item)
      {
        //is this a data item ?        
        if(substr($itemkey,0,$pre_len) == $pre)
        {
          $data[substr($itemkey,$pre_len)] =$item;
        }
        //is this a olddata item
        elseif(substr($itemkey,0,$oldpre_len) == $oldpre)
        {
          $olddata[substr($itemkey,$oldpre_len)] = $item;
        }
        //is this a weekdate item?
        elseif(substr($itemkey,0,$weekdates_len) == "weekdate_")
        {
          $datearr[substr($itemkey,$weekdates_len)] = $item;
        }
      }
    }
  
    /**
     * Get planned projects for a specified userid and a date     
     *     
     * @return array projectids
     */
    function getUserPlanningProjectIds($userid, $date)
    {
      /* @var $team_employee_node atkNode */
      /* @var $team_node atkNode */
      //get the teamid this user belongs to.
      $team_employee_node = &atkGetNode("employee.team_employee");
      $team_node = &atkGetNode("employee.team");
      $monday = dateutil::startOfWeek($date);
      $team_emplyee_records = $team_employee_node->selectDb("team_employee.employeeid='$userid'");      
      $teamid = 0;
      
      if(count($team_emplyee_records))      
        $teamid = $team_emplyee_records[0]["teamid"]["id"];
            
      if($teamid != 0)
      {
        /* @var $query atkQuery */
        $db     = &atkGetDb();
        $query  = &new atkQuery();
  
        $query->addField("projectid","","teamplanning_planning","",true);        
        $query->addTable("teamplanning_planning");
        $query->addCondition("teamid=$teamid");
        $query->addCondition("date='$monday'");        
        $records = $db->getrows($query->buildSelect());
        atk_var_dump($records,"<h1>found records on planning:</h1>");
      }
      
      $projectids = array();
      foreach($records as $rec)
      {
        $projectids[] = $rec["projectid"];
      }
      
      return $projectids;      
    }
  }
?>