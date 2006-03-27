<?php
  

  // Load the used relation and attribute classes
  useattrib("atkattribute");
  useattrib("atkdateattribute");
  useattrib("atkdatetimeattribute");
  userelation("atkmanytoonerelation");
  useattrib("atklistattribute");
  useattrib("atkdummyattribute");

  define("CONFLICT_WEEKPLAN_EXCEEDS_CURRENTPLAN", "weekplan_exceeds_currentplan"); // phaseid
  define("CONFLICT_PLANNED_EXCEEDS_TEAM_CAPACITY", "planned_exceeds_team_capacity"); // phaseid
  define("CONFLICT_BOOKED_NEARS_PLANNED", "booked_nears_planned"); // phaseid
  define("CONFLICT_BOOKED_EXCEEDS_PLANNED", "booked_exceeds_planned"); // phaseid
  define("CONFLICT_EMPLOYEE_UNDERPLANNED", "employee_underplanned"); // subjectid
  define("CONFLICT_PLANNED_EXCEEDS_EMPLOYEE_CONTRACT", "planned_exceeds_employee_contract"); // phaseid, subjectid, week


  
  class planningconflictlog extends atkNode
  {
    /**
     * planningconflictlog constructor
     */
    function planningconflictlog()
    {
      // Call the parent constructor
      $this->atkNode("planningconflictlog");
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkDateTimeAttribute("timestamp", AF_OBLIGATORY));
      $this->add(new atkAttribute("type", AF_OBLIGATORY));
      $this->add(new atkListAttribute("status", array("new", "persistent", "resolved", "ignored", "overruled"), "", AF_OBLIGATORY|AF_LIST_NO_OBLIGATORY_NULL_ITEM));
      $this->add(new atkManyToOneRelation("phaseid", "project.phase", AF_HIDE|AF_FORCE_LOAD));
      $this->add(new atkManyToOneRelation("activityid", "project.activity", AF_HIDE));
      $this->add(new atkManyToOneRelation("actorid", "employee.employee"));
      $this->add(new atkManyToOneRelation("subjectid", "employee.employee", AF_HIDE|AF_FORCE_LOAD));
      $this->add(new atkDateAttribute("conflictdate", "", "", 0, 0, AF_HIDE|AF_FORCE_LOAD));
      $this->add(new atkDummyAttribute("resolution", ""));
      $this->setTable("planning_conflict");

      $this->addFilter("planning_conflict.status IN ('new', 'persistent')");

    }

    /**
     * Displays the conflict type as a human readable conflict text.
     *
     * @param unknown_type $record
     * @param unknown_type $mode
     */
    function type_display($record, $mode)
    {
      $result= "";
      switch($record["type"])
      {
        case CONFLICT_BOOKED_EXCEEDS_PLANNED:
        {
          $phaseinfo = $this->getPhaseInfo($record["phaseid"]["id"]);
          return "Geboekte tijd '".$phaseinfo["projectname"]."'/'".$phaseinfo["name"]."' overschrijdt huidige planning";
        }
        case CONFLICT_BOOKED_NEARS_PLANNED:
        {
          $phaseinfo = $this->getPhaseInfo($record["phaseid"]["id"]);
          return "Geboekte tijd '".$phaseinfo["projectname"]."'/'".$phaseinfo["name"]."' dreigt huidige planning te overschrijden";
        }
        case CONFLICT_WEEKPLAN_EXCEEDS_CURRENTPLAN:
        {
          $phaseinfo = $this->getPhaseInfo($record["phaseid"]["id"]);
          return "De weekplanning van '".$phaseinfo["projectname"]."'/'".$phaseinfo["name"]."' overschrijdt de huidige urenbegroting";
        }
        case CONFLICT_PLANNED_EXCEEDS_EMPLOYEE_CONTRACT:
        {
          $empinfo = $this->getEmployeeInfo($record["subjectid"]["id"]);
          return "De weekplanning van medewerker '".$empinfo["lastname"].", ".$empinfo["firstname"]."' is in week ".$this->getWeek($record["conflictdate"])." voller dan contractueel toegestaan.";
        }
        // etc.
      }
      //return $result;
      return $record["type"];
    }
    
    function getWeek($arrdate)
    {
      atkimport("module.utils.dateutil");
      include_once("achievotools.inc");
      return weekstamp(dateutil::arr2stamp($arrdate));
    }

    function resolution_display($record, $mode)
    {
      $result= "<ul>";
      switch($record["type"])
      {
        case CONFLICT_BOOKED_EXCEEDS_PLANNED:
        case CONFLICT_BOOKED_NEARS_PLANNED:
        {
          $result.="<li>".$this->meetingUrl($record["phaseid"]["id"]);
          $result.="<li>".$this->hourSurveyUrl($record["phaseid"]["id"]);
          $result.="<li>".$this->editPhaseUrl($record["phaseid"]["id"]);
          break;
        }
        case CONFLICT_WEEKPLAN_EXCEEDS_CURRENTPLAN:
        {
          $result.="<li>".$this->editWeekplanUrl($record["phaseid"]["id"]);
          $result.="<li>".$this->editPhaseUrl($record["phaseid"]["id"]);
          break;
        }
        case CONFLICT_PLANNED_EXCEEDS_EMPLOYEE_CONTRACT:
        {
          $result.="<li>Alternatieve medewerkers:".$this->getAlternativeEmployees($record["subjectid"]["id"], $record["conflictdate"]);
          break;
        }
        // etc.
      }
      $result.="</ul>";
      return $result;
    }
    
    function getAlternativeEmployees($employee_id, $conflictdate)
    {
      // @todo: return list of all employees except the one with employee_id, that  have
      // less planned items than their contract allows.
      return "";
    }

    function hourSurveyUrl($phase_id)
    {
      return href(dispatch_url("reports.hoursurvey", "report", array("phaseid"=>$phase_id)), "Bekijk geboekte uren", SESSION_NESTED);
    }

    function editPhaseUrl($phase_id)
    {
      return href(dispatch_url("project.phase", "edit", array("atkselector"=>"phase.id=".$phase_id)), "Werk planning bij", SESSION_NESTED);
    }
    
    function editWeekplanUrl($phase_id)
    {
      return href(dispatch_url("weekplanning.weekplanning_perproject", "admin"), "Werk weekplanning bij", SESSION_NESTED);
    }


    function meetingUrl($phase_id)
    {
      $phaseinfo = $this->getPhaseInfo($phase_id);
      return href(dispatch_url("calendar.schedule", "add", array("title"=>"Project meeting ".$phaseinfo["projectname"],
                                                                 "description"=>"Geboekte tijd '".$phaseinfo["name"]."' dreigt planning te overschrijden")), "Beleg meeting", SESSION_NESTED);
    }



    function getPhaseInfo($phase_id)
    {
      // Return empty info if empty phase id given
      if (empty($phase_id))
        return array("name"=>"", "projectname"=>"");

      static $s_info = array();
      if (!isset($s_info[$phase_id]))
      {
        $db = &$this->getDb();
        $recs = $db->getrows("SELECT ph.name as name, p.name as projectname FROM phase ph, project p WHERE p.id = ph.projectid AND ph.id=".$phase_id);
        $s_info[$phase_id] = $recs[0];
      }
      return $s_info[$phase_id];
    }
    
    function getEmployeeInfo($employee_id)
    {
      static $s_info = array();
      if (!isset($s_info[$employee_id]))
      {
        $db = &$this->getDb();
        $recs = $db->getrows("SELECT p.lastname, p.firstname FROM person p WHERE p.id = ".$employee_id);
        $s_info[$employee_id] = $recs[0];
      }
      return $s_info[$employee_id];
    }


    /**
     * Set up default values for this node
     *
     * @return array Array containing Attributename=>Initialvalue pairs
     */
    function initial_values()
    {
      $user = getUser();
      return array(
        "timestamp" => array("year" => date("Y"), "month" => date("m"), "day" => date("d"), "hours" => date("H"), "minutes" => date("i")),
        "actor" => $user,
        "status" => "new"
      );
    }

    // todo, ook een link toevoegen die de mogelijke oplossingen toont. (een soort 'suggestions' link), bijv als custom actie.

    /**
     * Return record descriptor (for use in relations etc.)
     *
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      // @todo: replace this with proper descriptor fields.
      return "[type]";
    }

    function rowColor($record)
    {
      switch ($record["status"])
      {
        case "new": return COLOR_ERROR;
        case "persistent": return COLOR_WARNING;
      }
    }

    function checkConflictBooked($phase_id)
    {
      $id = $this->findConflict(CONFLICT_BOOKED_EXCEEDS_PLANNED, $phase_id);
      $is_conflict = $this->detectBookedExceedsPlanned($phase_id);
      if ($id!==false && $is_conflict)
      {
        $this->confirmConflict($id);
      }
      else if ($id!==false && !$is_conflict)
      {
        $this->revokeConflict($id);

        // might be regression to 'nears'.
        $is_conflict = $this->detectBookedNearsPlanned($phase_id);
        if ($is_conflict)
        {
          $this->createConflict(CONFLICT_BOOKED_NEARS_PLANNED, $phase_id);
        }
      }
      else if ($is_conflict)
      {
        $this->createConflict(CONFLICT_BOOKED_EXCEEDS_PLANNED, $phase_id);
        // exceeds supercedes nears..
        $id = $this->findConflict(CONFLICT_BOOKED_NEARS_PLANNED, $phase_id);
        if ($id!==false) $this->overruleConflict($id);
      }
      else
      {
        // there's no conflict_booked_exceeds_planned conflict. Check booked_nears_planned
        $id = $this->findConflict(CONFLICT_BOOKED_NEARS_PLANNED, $phase_id);
        $is_conflict = $this->detectBookedNearsPlanned($phase_id);
        if ($id!==false && $is_conflict)
        {
          $this->confirmConflict($id);
        }
        else if ($id!==false && !$is_conflict)
        {
          $this->revokeConflict($id);
        }
        else if ($is_conflict)
        {
          $this->createConflict(CONFLICT_BOOKED_NEARS_PLANNED, $phase_id);
        }
      }
    }
    
    function checkConflictPlanned($phase_id)
    {
      $id = $this->findConflict(CONFLICT_WEEKPLAN_EXCEEDS_CURRENTPLAN, $phase_id);
      $is_conflict = $this->detectWeekplanExceedsCurrentplan($phase_id);
      if ($id!==false && $is_conflict)
      {
        $this->confirmConflict($id);
      }
      else if ($id!==false && !$is_conflict)
      {
        $this->revokeConflict($id);        
      }
      else if ($is_conflict)
      {
        $this->createConflict(CONFLICT_WEEKPLAN_EXCEEDS_CURRENTPLAN, $phase_id);        
      }      
    }
    
    function checkConflictEmployee($employee_id, $monday)
    {
      $id = $this->findConflict(CONFLICT_PLANNED_EXCEEDS_EMPLOYEE_CONTRACT, NULL, $employee_id, $monday);
      $is_conflict = $this->detectWeekplanExceedsContract($employee_id, $monday);
      if ($id!==false && $is_conflict)
      {
        $this->confirmConflict($id);
      }
      else if ($id!==false && !$is_conflict)
      {
        $this->revokeConflict($id);        
      }
      else if ($is_conflict)
      {
        $this->createConflict(CONFLICT_PLANNED_EXCEEDS_EMPLOYEE_CONTRACT, NULL, $employee_id, $monday);        
      }      
    }


    function findConflict($type, $phase_id=NULL, $employee_id=NULL, $monday=NULL)
    {
      atkdebug("looking for existing conflict of type $type in phase $phase_id, employee $employee_id, monday $monday");
      
      $filter = array("type='".$type."'");
      if ($phase_id!=NULL) $filter[] = "phaseid=".$phase_id;
      if ($employee_id!=NULL) $filter[] = "subjectid=".$employee_id;
      if ($monday!=NULL) $filter[] = "conflictdate='".$monday."'";
      
      
      $recs = $this->selectDb(implode(" AND ", $filter), "", "", "", array("id"));
      if (count($recs))
      {
        atkdebug("found!");
        return $recs[0]["id"];
      }
      atkdebug("notfound!");
      return false;
    }
    
    function detectWeekplanExceedsContract($employee_id, $monday)
    {
      $db = &$this->getDb();
      $data_wp = $db->getrows("SELECT 
                                 wp.employeeid,                               
                                 SUM(wp.minutes) as planning
                               FROM                                
                                 weekplanning_planning wp
                               WHERE                                 
                                 wp.monday = '$monday'
                                 AND wp.employeeid = $employee_id
                               GROUP BY 
                                 wp.employeeid");
      
      if (!count($data_wp)) return false; // no planning, no conflict
      
      $data_c = $db->getrows("SELECT 
                              c.userid, 
                              SUM(c.uc_hours) as contracthours
                            FROM
                              usercontract c
                            WHERE                              
                              c.startdate <= '$monday'
                              AND (c.enddate IS NULL OR c.enddate >= '$monday')                              
                              AND c.userid = $employee_id
                            GROUP BY 
                              c.userid");
      
      if (!count($data_c)) return true; // planning, but no contract, always conflict
      
      return ($data_c[0]["contracthours"]*60)<$data_wp[0]["planning"];
    }

    function detectWeekplanExceedsCurrentplan($phase_id)
    {
      $db = &$this->getDb();
      $data = $db->getrows("SELECT
                              ph.id,
                              ph.current_planning as currentplan,
                              SUM(wp.minutes) as weekplan
                            FROM
                              phase ph,
                              weekplanning_planning wp
                            WHERE
                              wp.phaseid = ph.id
                              AND ph.id = $phase_id
                            GROUP BY
                              ph.id, ph.current_planning");
      return (count($data) && $data[0]["currentplan"]<$data[0]["weekplan"]);
    }
    
    function detectBookedExceedsPlanned($phase_id)
    {
      $db = &$this->getDb();
      $data = $db->getrows("SELECT
                              ph.id,
                              ph.current_planning as planned,
                              SUM(h.time) as booked
                            FROM
                              phase ph,
                              hours h
                            WHERE
                              h.phaseid = ph.id
                              AND ph.id = $phase_id
                            GROUP BY
                              ph.id, ph.current_planning");
      return (count($data) && $data[0]["planned"]<$data[0]["booked"]);
    }


    function detectBookedNearsPlanned($phase_id)
    {
      $db = &$this->getDb();
      $data = $db->getrows("SELECT
                              ph.id,
                              ph.current_planning as planned,
                              SUM(h.time) as booked
                            FROM
                              phase ph,
                              hours h
                            WHERE
                              h.phaseid = ph.id
                              AND ph.id = $phase_id
                            GROUP BY
                              ph.id, ph.current_planning");
      return (count($data) && ($data[0]["planned"]*0.95)<=$data[0]["booked"]);
    }

    function overruleConflict($id)
    {
      $this->updateStatus($id, "overruled");
    }

    function revokeConflict($id)
    {
      $this->updateStatus($id, "resolved");
    }

    function confirmConflict($id)
    {
      $this->updateStatus($id, "persistent");
    }

    function updateStatus($id, $newstatus)
    {
      $db = &$this->getDb();
      $db->query("UPDATE weekplanning_conflict SET status='$newstatus' WHERE id=$id");
    }

    function createConflict($type, $phase_id=NULL, $employee_id=NULL, $conflictdate=NULL)
    {
      $db = &$this->getDb();
      $user = getUser();
      if ($phase_id==NULL) $phase_id = "NULL";
      if ($employee_id==NULL) $employee_id = "NULL";
      if ($conflictdate==NULL) { $conflictdate = "NULL"; } else { $conflictdate = "'$conflictdate'"; } // must quote date
      $db->query("INSERT INTO weekplanning_conflict (id, timestamp, type, status, phaseid, subjectid, conflictdate, actorid)
                     VALUES(".$db->nextid("weekplanning_conflict").",".$db->func_now().", '".$type."', 
                              'new', ".$phase_id.", ".$employee_id.", ".$conflictdate.", ".$user["id"].")");
    }


  }
?>