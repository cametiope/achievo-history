<?php

include_once("achievotools.inc");
atkimport("atk.ui.atkui");
atkimport("module.utils.dateutil");
useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atkdurationattribute");
useattrib("atklistattribute");
useattrib("atktimeattribute");
useattrib("timereg.taskattribute");
useattrib("atkdummyattribute");
userelation("atkmanytoonerelation");

useattrib("atkdatetimeattribute");

class hours extends atkNode
{
  var $m_lock = "";
  var $m_viewdate = "";
  var $m_weekview = "";
  var $m_user = "";
  var $m_errors = "";
  var $m_sessionData = "";
  //var $m_insertmode = ""; // can be set to 'day' or 'multi'  
  
  function hours($type="hours", $flags=0)
  { 
    //@todo: remove unneeded fields from db
        
    $this->atkNode($type,$flags);
    $this->add(new atkAttribute("id",AF_PRIMARY|AF_HIDE));
    $this->add(new atkDateAttribute("taskdate","F d Y","d F Y", 0, date("Ymd"),($this->m_weekview?0:AF_HIDE_LIST)|AF_HIDE));
    
    /* The dummy attributes use the information in the taskAttribute */    
    $this->add(new taskAttribute("taskid",AF_HIDE|AF_FORCE_LOAD));
    $this->add(new atkDummyAttribute("projectandphase","unknown project- and phasename"));
    $this->add(new atkDummyAttribute("activity","unknown activity name"));
    
    $this->add(new atkManyToOneRelation("userid","employee.employee",AF_HIDE_LIST));
    $relation = &new atkManyToOneRelation("workperiod","timereg.workperiod", AF_HIDE_LIST);
    $relation->setHideWhenEmpty(true);
    $this->add($relation);
    
    $this->add(new atkTextAttribute("remark",0 , 0));
    $this->add(new atkDurationAttribute("time"));
    $this->add(new atkDateAttribute("entrydate",AF_HIDE));    

    $this->setOrder("hours.id"); // Sort by order of input..
    $this->setTable("hours","hours");
  }  
  
  /**
   * Get the viewdate in the following order of priority:
   * postvars, session, current date
   *      
   * @return string viewdate
   */
  function getViewDate()
  {
    /* @var $sessionManager atkSessionManager */
    $sessionManager = &atkGetSessionManager();
    
    //first we try the viewdate in the postvars
    $viewdate = $this->m_postvars["viewdate"];    
    
    //if we do not have a valid viewdate, try the session
    if($viewdate=="" || !isset($viewdate))    
      $viewdate = $sessionManager->pageVar("viewdate");
    
      //if we do not have a valid viewdate, set it to today.
    if($viewdate == "" || !isset($viewdate))  
      $viewdate = date("Y-m-d");    
    
    //once we have a date, put it in the session.
    $sessionManager->pageVar("viewdate",$viewdate);
    
    if(is_array($viewdate))    
    {
      //make sure the viewdate is in the correct format.
      return $viewdate["year"]."-".str_pad($viewdate["month"],2,"0",STR_PAD_LEFT)."-".str_pad($viewdate["day"],2,"0",STR_PAD_LEFT);
    }
    
    return $viewdate;
  }
  
  /**
   * Get the active user in the following order of priority:
   * postvars, session, currently logged in user
   *      
   * @return int userid
   */
  function getActiveUser()
  {
    /* @var $sessionManager atkSessionManager */
    $sessionManager = &atkGetSessionManager();
    
    $userid = $this->m_postvars["userid"];
    
    //if we do not have a user in the postvars, try the session.
    if(!isset($userid) || $userid=="")
      $userid = $sessionManager->pageVar("viewuser");
    
    //if we do not have a valid user by now, select 'me'
    if($userid=="")
    {     
      $user = &getUser();
      $userid = $user["id"];
    }
    
    return $userid;
  }
  
  /**
   * Get the viewmodus 'weekview' in the following order of priority:
   * postvars, session, 0
   *      
   * @return int weekview
   */
  function getWeekView()
  {
    /* @var $sessionManager atkSessionManager */
    $sessionManager = &atkGetSessionManager();
    
    $weekview = $this->m_postvars["weekview"];
    
    //if we do not have a weekview in the postvars, try the session.
    if(!isset($weekview) || $weekview=="")
      $weekview = $sessionManager->pageVar("weekview");
    
    //if we do not have a valid $weekview by now, select 0
    if($weekview=="") $weekview = 0;
    
    //once we have a weekview, put it in the session.
    $sessionManager->pageVar("weekview",$weekview);
        
    return $weekview;
  }
  
  /**
   * Get the page data such as viewdate, user, weekview, errors and sessionData.         
   */
  function getPageData()
  {        
    $this->m_viewdate     = $this->getViewDate();    
    $this->m_user         = $this->getActiveUser();       
    $this->m_weekview     = $this->getWeekView();

    $this->m_errors       = sessionLoad("hours_errors","default");
    $this->m_sessionData  = sessionLoad("data","default");
       
    //we cannot work with the NULL values
    if($this->m_errors == "NULL")       $this->m_errors = "";
    if($this->m_sessionData == "NULL")  $this->m_sessionData = "";    
  }
  
  function addPage(&$handler,$record=null)
  {
    $this->getPageData();    
    $page = &$this->getPage();    
    $page->register_scriptcode($this->getScript());
    $output = "";
    //report errors to user
    if(isset($this->m_errors["obligatoryfields"]))
    {
      $output.= '<font style="color:red">'.atktext('couldnotsavedueto')."<br /><br />";
      foreach($this->m_errors["obligatoryfields"] as $err)      
        $output.= $err."<br />";      
      $output.= '</font><br />';
    }

    $output.= $this->renderData($this->getData());
    
    // Get the singleton instance of the ui renderer
	  $ui = &atkui::getInstance();    
	  return $ui->renderBox(array("title"=>atktext("hours"),"content"=>$output));
  }
  
  function action_admin(&$handler)
  { 
    $this->getPageData();
        
    $this->setLock();

    $this->addFilter("hours.userid='$this->m_user'");

    $output = "";
    //if we are not in weekview mode    
    if ($this->m_weekview!=1)
    {
      if ($this->m_lock=="") $output.= $handler->invoke("addPage");
      $this->addFilter("taskdate='$this->m_viewdate'");
      $output.= $handler->invoke("adminPage");
    }
    else  //weekview mode
    {
      if (atkconfig("timereg_week_bookable")==true && $this->m_lock=="") $output.= $handler->invoke("addPage");      
      $output.= $this->weekview();
    }

    $page = &$this->getPage();
    $this->addStyle("style.css");
    $page->addContent($this->renderActionPage("admin", $output));
  }
  
  function adminHeader()
  {
    if ($this->m_weekview!=1)
    {
      $viewdatestamp  = adodb_mktime(0,0,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4));
      $yesterday      = date('Y-m-d',$viewdatestamp-(86400-3601));   // we substract 3601 seconds to account for Daylight Savings Time      
      $tomorrow       = date('Y-m-d',$viewdatestamp+86400+3601);      // we add 3601 seconds to account for Daylight Savings Time
      $today          = date('Y-m-d');
      
      $output=  '<form name="dayview" method="post" action="dispatch.php">';
      $output.= session_form();
      $output.= '<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">';
      $output.= '<tr>
                  <td align="left" valign="top">'
                    .href('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$yesterday, atktext("previousday"));
      
      // If current is not today we can also move forward
      if ($this->m_viewdate<date('Y-m-d'))
      {        
        $output.='&nbsp;&nbsp;'.href('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$tomorrow, atktext("nextday"));
        $output.='&nbsp;&nbsp;'.href('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$today, atktext("today", "", "houradmin"));
      }
      
      $output.= '&nbsp;&nbsp;'.href('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$this->m_viewdate.'&weekview=1', atktext("gotoweekview"));
      $output.='</td><td align="right" valign="top">';
      
      $output.= $this->getUserSelect().'&nbsp;';
      $output.= $this->getDateJumper();      
      $output.='&nbsp;<input type="submit" value="'.atktext("goto").'">';
      $output.='</td></tr></table></form>';
      $output.='<b>'.atktext(strtolower(date("l", $viewdatestamp)))." ".atktext(strtolower(date("F", $viewdatestamp)))." ".date("d, Y", $viewdatestamp).'</b>'.$this->getLockIcon();
    }
    return $output;
  }
  
  function adminFooter()
  {
    //@todo: fix overtime_balance    
    if ($this->m_weekview!=1)
    {
      /* @var $balancenode atkNode */
      $balancenode = &getNode("timereg.overtime_balance");
      $balance = $balancenode->getBalance(date("Y-m-d", adodb_mktime(0,0,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4))), $this->m_user);
      $output ='<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">';
      $output.='<tr><td align="left" valign="top">';
      $output.= href('dispatch.php?atknodetype=calendar.schedule&atkaction=admin&viewdate='.$this->m_viewdate, atktext("viewschedule"), SESSION_NESTED);
      $output.='</td><td align="right">'.atktext("current_balance").': '.time_format((60*$balance["balance"]),true).'&nbsp;</td></tr></table>';
      return $output;
    }
    else
    {
      return "";
    }
  }

  function getDateJumper()
  {
    $dummy_rec = array("viewdate"=>$this->getInitialDate());    
    $datebox = new atkDateAttribute("viewdate","F d Y","d F Y",0, date("Ymd"),AF_OBLIGATORY);
    return $datebox->edit($dummy_rec);
  }
  
  function weekview()
  {
    if ($this->m_viewdate!="")
    {
      $viewtime = mktime(12,12,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4));
    }
    else
    {
      $this->m_viewdate = date("Y-m-d");
      $viewtime = time();
    }
    $this->addStyle("style.css");

    return $this->getWeekviewData($viewtime);

  }
  
  function getWeekviewData($viewtime)
  {
    $userid = $this->m_user;
    $weekday = strftime("%w",$viewtime);
    
    if ($weekday==0) $weekday=7;

    //for each day of the week
    for ($i=0;$i<=8;$i++)
    {
      $weekdates[$i]['date']=date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));
      $weekdates[$i]['day']=atktext(strtolower(date("D" ,$viewtime-(86400*($weekday-$i)))));
      $weekdates[$i]['stamp']=$viewtime-(86400*($weekday-$i));
    }

    $week       = array();
    $projtotals = array();
    $daytotals  = array();

    //$data = $this->selectDb("taskdate >= '".$weekdates[1]['date']."' AND taskdate <= '".$weekdates[7]['date']."' AND hours.userid = '$userid'","","","");
    atkimport('atk.db.atkquery');    
    $db     = &atkGetDb();    
    $query  = &new atkQuery();
    
    //get the ids of the most recent projects
    $query->addTable("hours");
    $query->addField("id AS projectid","","project");
    $query->addField("id AS phaseid","","phase");
    $query->addField("name AS projectname","","project");
    $query->addField("name AS phasename","","phase");
    $query->addField("time","","hours");
    $query->addField("taskdate","","hours");
    $query->addJoin("task","task","hours.taskid=task.id",true);
    $query->addJoin("activity","activity","task.refid=activity.id",true);
    $query->addJoin("phase","phase","activity.phaseid=phase.id",true);
    $query->addJoin("project","project","phase.projectid=project.id",true);
    $query->addCondition("task.tasktype='activity'");
    $query->addCondition("hours.userid='".$this->m_user."'");
    $query->addCondition("project.status='active'");
    $query->addCondition("taskdate >= '".$weekdates[1]['date']."' AND taskdate <= '".$weekdates[7]['date']."'");    
    $query->addOrderBy("hours.entrydate");
    
    $data = $db->getrows($query->buildSelect());    
    
    //reformat data for use in the table renderer
    for ($i=0,$_i=count($data);$i<$_i;$i++)
    {
      $rec = $data[$i];
      $key = $rec["projectname"]." - ".$rec["phasename"];      
      $date = $rec["taskdate"];
      $time = $rec["time"];
      $week[$key][$date]  += $time;
      $projtotals[$key]   += $time;
      $daytotals[$date]   += $time;
      $total              += $time;
    }    
    
    //get header links and text
    $output.= $this->weekviewHeader($weekdates, $viewtime);

    $table = &atknew("atk.utils.atktablerenderer");
    
    $weekdata = array();
    //create table header
    $weekdata[0][] = atktext("project")."/".atktext("phase");    
    for ($i=1;$i<=7;$i++)
    {
      if ($weekdates[$i]['date'] <= date("Y-m-d"))
      {
        $url = 'dispatch.php?viewdate='.$weekdates[$i]['date']."&weekview=0";
        $weekdata[0][] = href($url,$weekdates[$i]['day']).'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')';
      }
      else
      {
        $weekdata[0][] = $weekdates[$i]['day'].'<br>('.substr($weekdates[$i]['date'],5,2).'-'.substr($weekdates[$i]['date'],8,2).')';
      }
      $table->setColAlignment($i, TBL_CENTER);
    }
    $weekdata[0][] = atktext('total');

    //create table records
    $row = 1;
    while (list($proj, $times)=each($week))
    {
      $weekdata[$row][] = $proj;
      for ($i=1;$i<=7;$i++)
      {
        $weekdata[$row][] = time_format($times[$weekdates[$i]['date']]);
      }
      $weekdata[$row][] = time_format($projtotals[$proj]);
      $row++;
    }
    $weekdata[$row][] = atktext('total');

    //color rows red if there are more hours declared than the config overtimetreshold says.
    for ($i=1;$i<=7;$i++)
    {
      $color = ($daytotals[$weekdates[$i]['date']]>atkconfig("overtimethreshold")) ? "#FF0000" : "#000000";      
      $weekdata[$row][] = '<font color="'.$color.'">'.time_format($daytotals[$weekdates[$i]['date']]).'</font>';
    }
    $weekdata[$row][] = time_format($total);

    //render data    
    $output.= $table->render($weekdata, TBL_FOOTER|TBL_DATA, "recordlist");
    $output.= $this->weekviewFooter($weekdates, $viewtime);    
    
    $ui = &atknew("atk.ui.atkui");
    return $ui->renderBox(array("title"=>$this->weekviewTitle($userid, $viewtime),
                                    "content"=>$output));

  }

  function weekviewTitle($userid, $viewtime)
  {
    return atktext("title_houradmin_weekview")." ".strftime("%V, %Y",$viewtime);
  }

  /**
   * One line description of the function
   *
   * Full description of the function
   *
   * @param array $weekdates
   * @param int $viewtime
   * @return type description
   */
  function weekviewHeader($weekdates, $viewtime)
  {
    $userid = $this->m_user;
    $user = &getUser($userid);    

    // First a line with previous and next week links..
    $output='<form name="weekview" method="post" action="dispatch.php">';
    $output.=session_form();
    $output.='<table border="0" cellpadding="0" cellspacing="0" width="100%" class="table">';    
    $output.='<tr><td valign="top" align="left">';    
    $output.= ''.href('dispatch.php?weekview=1&viewdate='.$weekdates[0]['date'], atktext('previousweek'));
    
    //if we are not in the current week or later, we can also go forward.
    if ($weekdates[8]['date'] <= date("Y-m-d"))
    {
      $output.='&nbsp;&nbsp;'.href('dispatch.php?weekview=1&viewdate='.$weekdates[8]['date'],atktext('nextweek'));
    }
    $output.= '&nbsp;&nbsp;'.href('dispatch.php?weekview=1&viewdate='.date("Y-m-d"), atktext('thisweek'));
    $output.= '&nbsp;&nbsp;'.href('dispatch.php?weekview=0&viewdate='.date("Y-m-d"), atktext('dayview'));

    if ($this->m_lock=="individual" && $this->allowed("unlock"))
    {
      $output.='&nbsp;&nbsp;'.href('dispatch.php?atkaction=unlock&userid='.$userid.'&viewdate='.$this->m_viewdate,'<b>'.atktext("unlock").'</b>');
    }
    
    // users may only lock weeks that are not already locked. they must have the proper permission
    // and, if configured, they may only lock a week once they have booked all their hours.
    else if ($this->m_lock=="" && $this->allowed("lock") && (atkconfig("timereg_incompleteweeklock") || $this->weekComplete($userid, $this->m_viewdate)))
    {
      $output.='&nbsp;&nbsp;'.href('dispatch.php?atkaction=lock&userid='.$userid.'&viewdate='.$this->m_viewdate,'<b>'.atktext("lock").'</b>');
    }

    $output.='</td><td valign="top" align="right">';
    $output.=$this->getDateJumper();
    $output.='&nbsp;<input type="submit" value="'.atktext("goto").'">';
    $output.='</td></tr></table></form>';

    $output.='<b>'.$user["firstname"].' '.$user["lastname"].' - '.atktext("week").' '.adodb_date('W, Y',$viewtime).'</b>'.$this->getLockIcon().'<br><br><br>';
    return $output;
  }
  
  /**
   * This function checks whether a week is 'compleet'. This means that the user
   * has booked the same ammount of hours (or more) as is specified in his contract.
   * Note: This is checked against the contract that was valid on the first day
   * of the week.
   */
  function weekComplete($userid, $date)
  {
    $db = &atkGetDb();

    $start  = startOfWeek($date);
    $end    = endOfWeek($date);

    // first get the total of contract hours
    $sql = "SELECT sum(uc_hours*60) as contract FROM usercontract WHERE startdate <= '$start'
                AND (enddate > '$start' OR enddate IS NULL) AND userid = '$userid'";
    $result = $db->getRows($sql);

    // get the total of time already registered this week
    $query = "SELECT sum(time) as totaal
              FROM
                hours
              WHERE
                hours.userid = '$userid'
                AND taskdate between '$start' and '$end'";
    $res = $db->getrows($query);
    if (count($res)==0 || count($result)==0)
    {
      // either no contract in this period, or no hours booked at all..
      return false;
    }
    else
    {
      return ($result[0]["contract"] - $res[0]["totaal"]<=0);
    }
  }
  
  /**
   * Can be overriden to add a weekviewdata footer
   *
   * @return String HTML Footer to be embedded in the weekviewdata box
   */
  function weekviewFooter($weekdates, $viewtime)
  {
  }
  
  /**
   * Check if the currently selected viewdate is locked   
   */
  function setLock()
  {
    $this->m_lock = $this->getLockType(strtolower($this->m_user),$this->m_viewdate);

    if ($this->m_lock != "")
    {
      // This week is locked..
      $this->addFlag(NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);
    }
  }
  
  function getLockType($userid, $date)
  {
    static $s_locks = array();

    $week = weekstamp(adodb_mktime(0,0,0,substr($date,5,2),
                                     substr($date,8,2),
                                     substr($date,0,4)));

    if ($s_locks[$userid][$week]=="")
    {
      $query = "SELECT
                  week, userid
                FROM
                  hours_lock
                WHERE
                  (userid = '$userid' OR userid IS NULL OR userid='')
                AND
                  week = '$week'";
      $db = &atkGetDb();
      $res = $db->getrows($query);

      if (count($res))
      {
        $all = false;
        for ($i=0;$i<count($res);$i++)
        {
          if (empty($res[$i]["userid"]))
          {
            $s_locks[$userid][$week] = "all";
            $all = true;
          }
        }

        // apparently not locked for everybody.
        if (!$all)
        {
          $s_locks[$userid][$week] = "individual";
        }
      }
      else
      {
        $s_locks[$userid][$week]="none";
      }
    }

    return ($s_locks[$userid][$week]=="none"?"":$s_locks[$userid][$week]);
  }
  
  function action_lock(&$handler)
  {
    $this->getPageData();
    if (atkconfig("lock_week_approval_required"))
      $this->lockWeek();
    else
      $this->lockWeek("1");
  }
  
  function lockWeek($approved=null)
  {
    $viewdate = $this->m_viewdate;
    $record = array();
    $record["userid"]["id"] = $this->m_user;
    $record["week"] = weekstamp(adodb_mktime(0,0,0,substr($viewdate,5,2),
                                   substr($viewdate,8,2),
                                   substr($viewdate,0,4)));
    $record["approved"] = $approved;
    $locknode = &getNode("timereg.hours_lock");
    $locknode->addDb($record);
    $this->redirect();
  }

  function action_unlock(&$handler)
  {
    $this->getPageData();
    $db = &atkGetDb();

//    $viewdate = $this->m_postvars["viewdate"];
//    $userid = $this->m_postvars["userid"];
    $week = weekstamp(adodb_mktime(0,0,0,substr($viewdate,5,2),
                                   substr($viewdate,8,2),
                                   substr($viewdate,0,4)));
    $query = "DELETE FROM hours_lock WHERE week = '$week' AND userid = '$userid'";
    $db->query($query);
    $this->redirect();
  }
    
  /**
   * When loading the taskid, the activityid is automatically loaded in the query
   *
   * @param array $record the datarecord
   * @param string $mode The action mode
   * @return string value of the activity attribute
   */
  function projectandphase_display($record, $mode)
  { 
    return $record["taskid"]["projectname"]." - ".$record["taskid"]["phasename"];
  }

  /**
   * When loading the taskid, the activityid is automatically loaded in the query
   *
   * @param array $record the datarecord
   * @param string $mode The action mode
   * @return string value of the activity attribute
   */
  function activity_display($record, $mode)
  { 
    return $record["taskid"]["activityname"];
  }
  
  function action_save(&$handler)
  { 
    $this->getPageData();
    
    /* @var $sessionManager atkSessionManager */    
    $sessionManager = &atkGetSessionManager();
    
    //get posted data
    $data     = $this->m_postvars["data"];
    $projects = $this->getDataFromCells($data);    
    
    //In this array we will store errors we find.
    $errors = array();
    
    //In this array we will store the items we need to add to the db.
    $storeList = $this->getChangedProjectItems($projects,$errors);    

    //if we have any errors, store the items
    if(count($errors))
    {
      //if we have an error, store the data in the session.    
      sessionStore('hours_errors',$errors);
      sessionStore('data',$data);
    }
    else //write data to database
    {      
      //if we don't have an error, make sure the datafields are empty
      sessionStore('hours_errors',"NULL");
      sessionStore('data',"NULL");
      $this->writeToDb($storeList);
    }
    
    //return to the admin/add page
    $this->redirect();
  }
  
  /**
   * Function determines what information that was filled in the form
   * actually needs updating. It returns a list with the data that needs to
   * be added to the database.
   *
   * @param array $projects projects
   * @param array $errors If any errors occur, they will be stored in this variable
   * @return array storelist
   */
  function getChangedProjectItems(&$projects, &$errors)
  {
    //the estimated time for the phase can be obligatory
    $estimatedtimeisobligatory = atkConfig("hours_obligatory_estimated_time", false);
    
    $storeList = array();
    
    foreach($projects as $project)
    { 
      //get the name for the project
      $projectname = $this->getProjectName($project["id"]);
      
      foreach($project["phase"] as $phase)
      { 
        //get the name for the phase.
        $phasename = $this->getPhaseName($phase["id"]);
                
        foreach($phase["activities"] as $activity)
        {
          //check if we need to store the hours for this activity
          if($activity["time"] > 0)
          { 
            //get the name for the activity.
            $activityname = $this->getActivityName($activity["id"]);
              
            //if the estimated phasetime is obligatory, check if it is provided (it is allowed to be 0)
            if($estimatedtimeisobligatory && (!isset($activity["estimated"]) || $activity["estimated"]==""))
            {
              $errmsg = atktext("project").": $projectname, "
                        .atktext("phase").": $phasename, "
                        .atktext("activity").": $activityname.";
              $errors["obligatoryfields"][] = $errmsg;              
            }
            else 
            { 
              //add it to the storeList
              $storeList[] = array(                            
                            "activityid"    =>$activity["id"],
                            "activityname"  =>$activityname,
                            "tasktype"      =>'activity',
                            "time"          =>$activity["time"],
                            "estimated"     =>$activity["estimated"],
                            "taskdate"      =>$this->m_viewdate,
                            "workperiod"    =>$activity["workperiod"],
                            "remark"        =>$activity["remark"]
                            );
            }
          }
        }
      }    
    }
    return $storeList;   
  }
  
  function writeToDb(&$storeList)
  {
    /* @var $db atkDB */
    /* @var $tasknode atkNode */
    /* @var $hoursnode atkNode */
    $tasknode   = &getNode("task.task");
    $hoursnode  = &getNode("timereg.hours");
    $db         = &atkGetDb();    
    $dt         = date("Y-m-d");
    $datearr    = explode("-",date("Y-m-d-H-i-s"));    
    $datetime   = array();    
    $datetime["year"]   = $datearr[0];
    $datetime["month"]  = $datearr[1];
    $datetime["day"]    = $datearr[2];
    $datetime["hours"]  = $datearr[3];
    $datetime["minutes"]= $datearr[4];
    $datetime["seconds"]= $datearr[5];
    
    foreach($storeList as $item)
    {      
      $tasktype = $item['tasktype'];
      
      //for now only the tasktype activity is supported
      if($tasktype == 'activity')
      { 
        $activityid   = $item["activityid"];
        $activityname = $item["activityname"];
        $taskdate     = $item["taskdate"];
        $workperiod   = $item["workperiod"];
        $time         = $item["time"];
        $estimated    = $item["estimated"];
        $remark       = $item["remark"];
        
        //does the currently logged in user have a task linked to the current activity?
        $taskrecords = $tasknode->selectDb("task.owner='".$this->m_user."'"." AND task.tasktype='$tasktype' AND task.refid='$activityid'");
        $taskid = 0;
        
        //if not --> create one          
        if(count($taskrecords) == 0)
        {
          atkdebug("this user does not yet have a task related to this activity.");            
          
          $taskid                       = $db->nextid('task');
          $newtask["id"]                = $taskid;
          $newtask["name"]              = $activityname;
          $newtask["owner"]             = array("id"=>$this->m_user);
          $newtask["createdate"]        = $dt;
          if($estimated != "")  //if the user provided an estimated time value, we store it.
            $newtask["current_planning"]  = $estimated;
          else 
            $newtask["current_planning"] = "";
          $newtask["editdatetime"]      = $datetime;
          $newtask["refid"]             = $activityid;
          $newtask["tasktype"]          = $tasktype;
          $tasknode->addDb($newtask, false);
        }
        else 
        {
          atkdebug("this user already has a task related to this activity");          
          $taskid = $taskrecords[0]["id"];          
          atkdebug("estimeded value = ".$estimated);
          if($estimated != "")    //if the user provided an estimated time value, we store it.
          { 
            $taskrecords[0]["editdatetime"]     = $datetime;
            $taskrecords[0]["current_planning"] = $estimated;
            $tasknode->updateDb($taskrecords[0],false);
          }          
        }
                        
        $newhour["id"]          = $db->nextid('hours');
        $newhour["userid"]      = array("id"=>$this->m_user);
        $newhour["taskid"]      = $taskid;
        $newhour["entrydate"]   = $dt;
        $newhour["taskdate"]    = $this->m_viewdate;
        $newhour["time"]        = $time;
        $newhour["workperiod"]  = array("id"=>$workperiod);
        $newhour["remark"]      = $remark;
        $hoursnode->addDb($newhour,true);       
      }
      else 
      {
        atkdebug("other tasktypes than 'activity' are not (yet) supported.");
      }
    }
  }
  
  function getProjectName($projectid)
  {    
    $node     = &getNode("project.project");
    $records  = $node->selectDb("project.id='$projectid'","","","","name");
    return $records[0]["name"];
  }
  
  function getPhaseName($phaseid)
  {
    $node    = &getNode("project.phase");
    $records = $node->selectDb("phase.id='$phaseid'","","","","name");
    return $records[0]["name"];
  }
  
  function getActivityName($activityid)
  {
    $node     = &getNode("project.activity");
    $records  = $node->selectDb("activity.id='$activityid'","","","","name");
    return $records[0]["name"];
  }
  
  function getDataFromCells(&$data)
  {
    //data is found in the following format:

    //cellname : 1_1_1_estimated -> value:   
    //cellname : 1_1_1_hours -> value: 0
    //cellname : 1_1_1_minutes -> value: 0
    //cellname : 1_1_1_workperiod -> value: 1
    //cellname : 1_1_2_hours -> value: 0
    //cellname : 1_1_2_minutes -> value: 0
    //cellname : 1_1_2_workperiod -> value: 1 
        
    $projects = array();
    
    // Loop through all cells
    foreach($data as $cell => $value)
    { 
      // Determine the projectid phaseid and activityid according to the cellname
      list($projectid, $phaseid, $activityid) = explode("_", $cell);
      $projects[$projectid]["id"] = $projectid;
      $projects[$projectid]["phase"][$phaseid]["id"] = $phaseid;
      $projects[$projectid]["phase"][$phaseid]["activities"][$activityid]["id"] = $activityid;  
      
      //estimated field
      if(strpos($cell,"estimated"))
      {                
        if($value != "" && isset($value))        
          $projects[$projectid]["phase"][$phaseid]["activities"][$activityid]["estimated"] = $value * 60;        
        else 
          $projects[$projectid]["phase"][$phaseid]["activities"][$activityid]["estimated"] = "";
      }
      //hours field
      elseif(strpos($cell,"hours"))
      {
        $projects[$projectid]["phase"][$phaseid]["activities"][$activityid]["time"] = ($value * 60);
      }
      //minutes field
      elseif(strpos($cell,"minutes"))
      {       
        $projects[$projectid]["phase"][$phaseid]["activities"][$activityid]["time"] += $value;
      }
      //workperiod field
      elseif(strpos($cell,"workperiod"))
      {       
        $projects[$projectid]["phase"][$phaseid]["activities"][$activityid]["workperiod"] = $value;
      }
      //remark field
      elseif(strpos($cell,"remark"))
      {
        $projects[$projectid]["phase"][$phaseid]["activities"][$activityid]["remark"] = $value;
      }
    }
    
    return $projects;
  }
  
  /**
   * Create two clickable images, one of them is always, visible, the
   * other one is not.
   *
   * @param string $id the unique identifier of the project or phase these images belong to
   * @param string $srcopen source of image
   * @param string $srcclose source of image
   * @param string $regopen regular expression to use when clicking on open/plus
   * @param string $regclose expression to use when clicking on close/minus
   * @return string rendered html with two clickable images.
   */
  function clickableImages($id, $srcopen, $srcclose, $regopen, $regclose)
  { 
    $openimageid = "img_open_$id";
    $closeimageid = "img_close_$id";
    
    $hrefopen = "javascript:openObject('projects','$regopen','$openimageid','$closeimageid')";
    $hrefclose = "javascript:closeObject('projects','$regclose','$openimageid','$closeimageid')";
    
    $imgopen  = '<img id="'.$openimageid.'" src="'.$srcopen.'" border="0">';
    $imgclose = '<img id="'.$closeimageid.'" style="display:none" src="'.$srcclose.'" border="0">';

    $images = "<a href=\"$hrefopen\">$imgopen</a>";
    $images.= "<a href=\"$hrefclose\">$imgclose</a>";
    return $images;    
  }
  
  /**
   * Converts time from minutes to hours:minutes
   *   
   * @param int $minutes minutes
   * @return string hours:minutes
   */
  function convertTime($minutes)
  {
    return floor($minutes/60).":".str_pad($minutes%60,2,"0",STR_PAD_LEFT);
  }
  
  /**
   * Get dropdownlists for hour regisitrations      
   *
   * @param string $id id to uniquely identify the dropdownlist
   * @return string html
   */
  function getHourRegistrationDropDownLists($id, $selectedHours, $selectedMinutes)
  { 
    $output = "";
    //hours
    $output.= '<select name="data['.$id.'_hours]" id="data'.$id.'_hours]">';
    foreach(range(0,10) as $hour)
    {
      $sel = ($selectedHours == $hour) ? "selected" : "";
      $output.= '<option value="'.$hour.'" '.$sel.'>'.$hour;
    }
    $output.= '</select>';
    
    $output.=":";
    
    //minutes
    $output.= '<select name="data['.$id.'_minutes]" id="data['.$id.'_minutes]">';    
    foreach (array(0,15,30,45) as $minute)
    {
      $sel = ($selectedMinutes == $minute) ? "selected" : "";
      $output.= '<option value="'.$minute.'" '.$sel.'>'.$minute;
    }
    $output.= '</select>';
    
    return $output;
  }
    
  /**
   * Get dropdownlists for workperiod selection
   *
   * @param string $id id to uniquely identify the dropdownlist
   * @return string html
   */
  function getWorkPeriodDropDownLists($id, $workperiods, $standardId, $selectedWorkperiod)
  {
    $selectedId = (isset($selectedWorkperiod) && $selectedWorkperiod != "") ? $selectedWorkperiod : $standardId;
    
    $output = '<select name="data['.$id.'_workperiod]" id="data['.$id.'_workperiod]">';
    foreach ($workperiods as $period)
    {
      $sel = ($period["id"] == $selectedId) ? "selected" : "";
      $name = $period["name"]." (".$period["starttime"]["hours"].":".$period["starttime"]["minutes"]."-"
              .$period["endtime"]["hours"].":".$period["endtime"]["minutes"].")";
      $output.= '<option value="'.$period["id"].'" '.$sel.'>'.$name;
    }
    $output.= '</select>';
    
    return $output;
  }
  
/**
 * Render projects, phases and activities.
 *
 * @todo: In the future we add tickets and other tasks
 * that a user may register time on.
 *
 * @param array $data Contains objects that the user can register hours on
 * @param array $sessionData If we had an error, the sessionData contains the values of the fields that were
 *                            allready filled in.
 * @return string rendered output.
 */
  function renderData(&$data)
  { 
    //@todo: add table renderer (perhaps expand the table renderer)
    //@todo: color projects red when there are no phases
    //@todo: color phases red when there are no activities
    
    $sessionData        = $this->m_sessionData;
    $initdisplay        = 'style="display:none;"';    
    $workperiods        = $this->getWorkPeriods();
    $standardworkperiod = $this->getStandardWorkPeriodId($workperiods);    
    $plusimagesrc       = atkconfig("atkroot").'atk/images/plus.gif';
    $minimagesrc        = atkconfig("atkroot").'atk/images/minus.gif';  
    $maxlines           = atkconfig("timereg_remark_lines", 2);
    $projectid = 0;        
    
    $output = '<form name="entryform" action="dispatch.php" method="post">';
    $output.= session_form();    
    $output.= '<input type="hidden" name="atkaction" value="save">';
    $output.= '<input type="hidden" name="atkerror" value="">';
    $output.= '<br />';
    
    $output.= '      
    <table class="recordlist" id="projects">
    <tr class="row1">
      <th>'."Projecten\Fases\Activiteiten".'</th>
      <th>'.atktext("initial_planning").'</th>
      <th>'.atktext("current_planning").'</th>      
      <th>'.atktext("spend_hours").'</th>
      <th>'.atktext("Gewerkte")."&nbsp;<br />".atktext("hours").":".atktext("minutes").'</th>
      <th>'.atktext("werktijd<br />periode").'</th>
      <th>'."Schatting".'</th>
      <th>'.atktext("remark").'</th>
    </tr>
    ';
    
    foreach($data["projects"] as $project)
    {
      $projectid        = $project["id"];
      $projectname      = $project["name"];      
      $rowid            = "$projectid";
      $regopen          = "^row_{$projectid}_[0-9]+$";
      $regclose         = "^row_{$projectid}_";      
      $images           = $this->clickableImages($projectid,$plusimagesrc,$minimagesrc, $regopen, $regclose);      
      $currentplanning = 0;
      
      if(!isset($project["current_planning"]) || $project["current_planning"] == "" || $project["current_planning"] == 0)      
        $project["current_planning"] = (isset($project["initial_planning"]) && $project["initial_planning"]!="") ? $project["initial_planning"] : 0;
        
      if($project["allactivities"] === false)
      {
        $currentplanning = $this->convertTime($project["calculated_current_planning"])." / ".$this->convertTime($project["current_planning"]);
      }
      else 
      {
        $currentplanning = $this->convertTime($project["calculated_current_planning"]);
      }
      $output.='        
        <tr class="row2" id="row_'.$rowid.'">
          <td>'.$images.'&nbsp;<b>'.$projectname.'</b></td>          
          <td align="center">'.$this->convertTime($project["initial_planning"]).'</td>
          <td align="center">'.$currentplanning.'</td>
          <td align="center">'.$this->convertTime($project["spend_hours"]).'</td>
          <td align="center">&nbsp;</td>
          <td align="center">&nbsp;</td>
          <td align="center">&nbsp;</td>
          <td align="center">&nbsp;</td>
        </tr>
      ';      
            
      foreach($project["phase"] as $phase)
      {
        $phaseid        = $phase["id"];
        $phasename      = $phase["name"];        
        $rowid          = "{$projectid}_$phaseid";        
        $reg            = "^row_{$projectid}_{$phaseid}_[0-9]+$";
        $images         = $this->clickableImages($projectid."_".$phaseid, $plusimagesrc, $minimagesrc, $reg, $reg);        
        $currentplanning = 0;
        
        if(!isset($phase["current_planning"]) || $phase["current_planning"] == "" || $phase["current_planning"] == 0)      
          $phase["current_planning"] = (isset($phase["initial_planning"]) && $phase["initial_planning"]!="") ? $phase["initial_planning"] : 0;
          
        if($project["allactivities"] === false)
        {
          $value1 = $phase["calculated_current_planning"];
          $value2 = $this->convertTime($phase["current_planning"]);
          $currentplanning = $this->convertTime($phase["calculated_current_planning"])." / ".$this->convertTime($phase["current_planning"]);
        }
        else 
        {
          $currentplanning = $this->convertTime($phase["calculated_current_planning"]);
        }
        $output.='          
          <tr class="row2" '.$initdisplay.' id="row_'.$rowid.'">
            <td>'.str_repeat("&nbsp;",5).$images.'&nbsp;'.$phasename.'</td>
            <td align="center">'.$this->convertTime($phase["initial_planning"]).'</td>
            <td align="center">'.$currentplanning.'</td>
            <td align="center">'.$this->convertTime($phase["spend_hours"]).'</td>
            <td align="center">&nbsp;</td>
            <td align="center">&nbsp;</td>
            <td align="center">&nbsp;</td>            
            <td align="center">&nbsp;</td>
          </tr>
        ';        
        
        foreach($phase["activities"] as $activity)
        {
          $activityid   = $activity["id"];
          $activityname = $activity["name"];
          $rowid        = "$projectid"."_$phaseid"."_$activityid";
          
          $output.='
          <tr class="row1" '.$initdisplay.' id="row_'.$rowid.'">
            <td>'.str_repeat("&nbsp;",10).$activityname.'</td>  
            <td align="center">&nbsp;</td>
            <td align="center">'.$this->convertTime($activity["current_planning"]).'</td>
            <td align="center">'.$this->convertTime($activity["spend_hours"]).'</td>
            <td align="center">'.$this->getHourRegistrationDropDownLists($rowid, $sessionData[$rowid.'_hours'], $sessionData[$rowid.'_minutes']).'</td>
            <td align="center">'.$this->getWorkPeriodDropDownLists($rowid,$workperiods,$standardworkperiod,$sessionData[$rowid.'_workperiod']).'</td>
            <td align="center"><input type="text" name="data['.$rowid.'_estimated]" id="data['.$rowid.'_estimated]" value="'.$sessionData[$rowid.'_estimated'].'" size="2"></input></td>
            <td align="center"><textarea name="data['.$rowid.'_remark]" id="data['.$rowid.'_remark]" cols="10" rows="'.$maxlines.'">'.$sessionData[$rowid.'_remark'].'</textarea></td>
          </tr>';
        }
      }      
    }
    //@todo: add filter for rights 'members_only' and 'alwaysvisibleintimereg'
    $targeturl = $_SERVER["PHP_SELF"]."?selectedprojectid=[id]&atklevel=".atkLevel();
    $select = dispatch_url("project.project", "select", array("atkfilter" => "project.status='active'", "atktarget" => $targeturl));
    
    //Footer
    $output.='
      <tr border="0">
        <td colspan="8" align="right">'.href($select,atktext("searchallprojects"),SESSION_NESTED, false).'</a></td>
      </tr>
      </table>';
    
    //button
    $output.= '<br /><input type="submit" class="btn_save" name="atknoclose" value="'.atktext("save").'"><br /><br />';
       
    $output.= '</form>';

    return $output;    
  }  
 
    /**
    * Get javascript code
    *    
    * @return string javscript
    */ 
  function getScript()
  {    
    $plusimagesrc = atkconfig("atkroot").'atk/images/plus.gif';
    $minimagesrc  = atkconfig("atkroot").'atk/images/minus.gif';
    
    $output = "
     //get_object is a function in atktools.js    
     function openObject(tablename,regexpression,imageopen,imageclose)
     {
      theRows     = get_object(tablename).rows;
      imgOpenObj  = get_object(imageopen);
      imgCloseObj = get_object(imageclose);
          
      reg=new RegExp(regexpression);
      
      //test all rows of the table.
      for (i=0;i<theRows.length;i++)
      {
        //if we find one that passes the test
        if (reg.test(theRows[i].id))
        {          
          imgOpenObj.style.display='none';
          imgCloseObj.style.display='';
          theRows[i].style.display='';          
        }
      }      
    }

    function closeObject(tablename,regexpression,imageopen,imageclose)
    {
      theRows = get_object(tablename).rows;      
      imgOpenObj  = get_object(imageopen);
      imgCloseObj = get_object(imageclose);                 
          
      reg=new RegExp(regexpression);
      
      //test all rows of the table.
      for (i=0;i<theRows.length;i++)
      {
        //if we find one that passes the test
        if (reg.test(theRows[i].id))
        {
          imgOpenObj.style.display='';
          imgCloseObj.style.display='none';
          theRows[i].style.display='none';       
        }
      }      
    }

    ";
    return $output;
  }
    

/**
 * Get all timeregistration data for this user to start registering hours.
 * 
 * @return array records with projects,phases,activities and tasks
 */
  function getData()
  {       
    $selectedprojectid = $this->m_postvars["selectedprojectid"];
    
    $data = array();
      
    //get all projects that this user has recently registered hours on.
    //And all projects that appear in the teamplanning for this user.
    $projects = $this->getProjects();    
    $data["projects"] = $projects;
    
    
    
    //if $selectedprojectid has a value, we came back from selecting a project.
    //select that project too. We put it at the bottom.
    if(isset($selectedprojectid) && $selectedprojectid!="")    
      $data["projects"][] = $this->getProject($selectedprojectid);
        
    //@todo: add tasks assigned to this user.

    return $data;
  }  
   
  
  function getProjects()
  {
    $recentids = $this->getRecentProjectIds();
    $planningids = $this->getPlanningProjects();    
    return $this->getProjectsByIds(array_unique(array_merge($recentids,$planningids)));    
  }
  
  function getPlanningProjects()
  {
    if(moduleExists("teamplanning"))
    {
      $user = &getUser();
      /* @var $node teamplanning */
      $node = atkGetNode("teamplanning.teamplanning");
      return $node->getUserPlanningProjectIds($user["id"],date("Ymd"));
    }
    else 
      return array();
  }
  
  function getRecentProjectIds()
  {
    atkimport('atk.db.atkquery');
    
    $db     = &atkGetDb();
    $query  = &new atkQuery();
    
    //get the ids of the most recent projects
    $query->addTable("hours");
    $query->addField("id","","project");
    $query->addJoin("task","task","hours.taskid=task.id",true);
    $query->addJoin("activity","activity","task.refid=activity.id",true);
    $query->addJoin("phase","phase","activity.phaseid=phase.id",true);
    $query->addJoin("project","project","phase.projectid=project.id",true);
    $query->addCondition("task.tasktype='activity'");
    $query->addCondition("hours.userid='".$this->m_user."'");
    $query->addCondition("project.status='active'");    
    $query->addGroupBy("project.id");
    $query->addOrderBy("hours.entrydate");    
    $sql = $query->buildSelect();
    
    //@todo: Make config for number of recent projects    
    //we now use 10
    $recentprojects = $db->getrows($sql,0,10);
    
    //get only the ids
    $recentprojectids = array();    
    foreach($recentprojects as $recentproject)
      $recentprojectids[] = $recentproject["id"];   
    
    return $recentprojectids;
  }
  /**
    * Get projects by an array of ids.
    *
    * @return array projects
    */ 
  function getProjectsByIds($projectids)
  {        
    atkimport('atk.db.atkquery');
    
    $db     = &atkGetDb();
    $query  = &new atkQuery();
    
    //get all data that belongs to these projects    
    $projects = array();
    /* @var $node atkNode */
    $node   = &getNode("project.project");    
    if($projectids != array())
    {
      //get the projects, phases and activities
      $projects = $node->selectDb("project.id IN (".implode(", ",$projectids).")","project.name","","",array("id","name","phase"));
      
      $mostrecentestimations = $this->_getMostRecentEstimations($projectids);
      
      //calculate some values
      for($i=0,$_i=count($projects);$i<$_i;$i++)
      {
        //get a reference to the project
        $project = &$projects[$i];
        
        $projects[$i]["initial_planning"] = 0;
        $projects[$i]["current_planning"] = 0;        
        $projects[$i]["spend_hours"] = 0;
        
        for($j=0,$_j=count($projects[$i]["phase"]);$j<$_j;$j++)
        {
          //get a reference to the phase
          $phase = &$projects[$i]["phase"][$j];
          $phase["spend_hours"] = "-";          
          
          if(isset($phase["initial_planning"]) && $phase["initial_planning"] != "")
          {            
            $projects[$i]["initial_planning"] += $phase["initial_planning"];
          }           

          //calculate current planning for project and phases
          if(isset($phase["current_planning"]) && $phase["initial_planning"] != "")
          {            
            $projects[$i]["current_planning"] += $phase["current_planning"];
          }

          //calculate spend_hours for project, phase and activities
          for($k=0,$_k=count($phase["activities"]);$k<$_k;$k++)
          {
            //get a reference to the activity
            $activity = &$phase["activities"][$k];

            //get registered hours for this phase.
            $query = &new atkQuery();
            $query->addField("sum(hours.time) as spend_hours");
            $query->addTable("hours");
            $query->addJoin("task","","hours.taskid=task.id",true);
            $query->addJoin("activity","","task.refid=activity.id",true);            
            $query->addCondition("task.tasktype='activity'");
            $query->addCondition("activity.id=".$activity["id"]);
            $spend_hours_activity = $db->getrows($query->buildSelect());
              
            $activityestimate = $mostrecentestimations[$project["id"]]["phases"][$phase["id"]]["activities"][$activity["id"]]["current_planning"];
                        
            $activity["spend_hours"]      = $spend_hours_activity[0]["spend_hours"];
            $activity["current_planning"] = $activityestimate;
            $phase["spend_hours"]         += $spend_hours_activity[0]["spend_hours"];
            $projects[$i]["spend_hours"]  += $spend_hours_activity[0]["spend_hours"];
          }
        }        
      }
    }
    
    //loop through all projects to recalculate the current_planning to show a range if needed
    for($i=0,$_i=count($projects);$i<$_i;$i++)
    {      
      $project = &$projects[$i];      
      $sumphases = 0;      
      $allphases = true;      
      for($j=0,$_j=count($project["phase"]);$j<$_j;$j++)
      {        
        $phase = &$project["phase"][$j];
        $sumactivity = 0;
        $allactivities = true;
        for($k=0,$_k=count($phase["activities"]);$k<$_k;$k++)
        {     
          $activity = &$phase["activities"][$k];
          if(!isset($activity["current_planning"]))          
            $allactivities = false;
          else          
          {
            $sumactivity += $activity["current_planning"];            
          }            
        }
        
        $phase["allactivities"] = $allactivities;        
        $phase["calculated_current_planning"] = $sumactivity;
        $sumphases += $sumactivity;
      }
      
      $project["allactivities"] = $allactivities;
      $project["calculated_current_planning"] = $sumphases;
    }    
    
    return $projects;
  }
  
  
  function getProject($projectid)
  {
    /* @var $node atkNode */
    $node = &getNode("project.project");
    $record = $node->selectDb("project.id='$projectid'","","","",array("id","name","phase"));
    return $record[0];    
  }
  
  /**
   * Get all the workperiods
   *
   * @return array workperiods
   */
  function getWorkPeriods()
  {
    /* @var $node atkNode */
    $node = &getNode("timereg.workperiod");
    return $node->selectDb();
  }
  
  /**
   * Get the id of the standard workperiod.
   *
   * @param array $workperiods All the workperiods
   * @return int the id of the standard workperiod
   */
  function getStandardWorkPeriodId($workperiods)
  { 
    switch(count($workperiods))
    {
      case 0:
        return "";
        break;
      case 1:
        //if there is only one workperiod, make it default.
        return $workperiods[0]["id"];
        break;
      default:
        foreach($workperiods as $period)
        {
          //search a workperiod that has the defaultrate set to 1.
          if($period["defaultrate"] == "1")
            return $period["id"];
        }        
    }
           
    //it appears to be, none of them is default. Lets select the first
    return $workperiods[0]["id"];
  }
   
  function postAdd($rec)
  {
    return $this->postAddOrUpdate($rec,"add");
  }

  function postUpdate($rec)
  { 
    return $this->postAddOrUpdate($rec,"edit");
  }
  
  function postAddOrUpdate($rec, $mode)
  {
    $this->getPageData();
    // if we add or update a record on a date that is not the currently viewed date, we jump to that date.
    //we also put it in the session.
    $sessionManager = &atkGetSessionManager();       
    $this->m_viewdate=$rec["taskdate"];
    $sessionManager->pageVar("viewdate",$this->m_viewdate);
    
    // update overtime balance
    $this->calculateNewBalance($rec, $mode);
    return true;
  }
  
  function postDelete($rec)
  {  
    $this->calculateNewBalance($rec, "delete");
    return true;
  }

  function calculateNewBalance($rec, $mode)
  {
    $balancenode = &getNode("timereg.overtime_balance");
    
    $userid       = $rec["userid"]["id"];
    if(is_array($rec["taskdate"]))
    {
      $year         = $rec["taskdate"]["year"];
      $month        = $rec["taskdate"]["month"];
      $activityday  = $rec["taskdate"]["day"];
    }
    else 
    {
      $year         = substr($taskdate,0,4);
      $month        = substr($taskdate,5,2);
      $activityday  = substr($taskdate,8,2);    
    }
    
    $day = date("Y-m-d", adodb_mktime(0,0,0,$month,$activityday,$year));
    $yesterday = date("Y-m-d", adodb_mktime(0,0,0,$month,$activityday-1,$year));    
    $balance = $balancenode->getBalance($day, $userid);
        
    // only update balance if balance was not set manually
    if ($balance["manual"] == 0)
    {
      if ($mode == "add")
      {
        // check if a record already exists for this day
        if ($balance["balance"] == atktext("not_inserted"))
        {
          // insert a new record with the balance of yesterday or 0
          $newbalance = $balancenode->getBalance($yesterday, $userid);          
          if ($newbalance["balance"] == atktext("not_inserted")) $newbalance["balance"] = 0;
          $balancenode->addOvertimeBalance($userid, $day, $newbalance["balance"]);
        }
      }

      $percentage = $balancenode->getPercentage($rec["workperiod"]["id"]);
      $hours = ($rec["time"]/60);

      if ($mode == "add") $newbalance = ($balance["balance"] + (($percentage/100) * $hours));
      elseif ($mode == "edit")
      {
        $oldpercentage = $balancenode->getPercentage($rec["atkorgrec"]["workperiod"]["id"]);
        $oldhours = $rec["atkorgrec"]["time"]/60;
        $newbalance = (($balance["balance"] - (($oldpercentage/100) * $oldhours)) + (($percentage/100) * $hours));
      }
      elseif($mode == "delete") $newbalance = ($balance["balance"] - (($percentage/100) * $hours));

      $balancenode->saveOvertimeBalance($rec["userid"]["id"], $day, $newbalance);
    }
    $manual = $balancenode->getLatestManualCorrection($rec["userid"]["id"], $yesterday);
    if ($manual != $yesterday)
    {
      $manualdate = date("Y-m-d", adodb_mktime(0,0,0,$manual["day"]["month"],$manual["day"]["day"],$manual["day"]["year"]));
    }
    else $manualdate = $yesterday;
    if ($manualdate < $day) $balancenode->deleteBalance($rec["userid"]["id"], $day);
  }
  
  function getUserSelect()
  {
    $res = "";
    
    if ($this->allowed("any_user"))
    {
      /* @var $node atkNode */
      $node = &getNode("employee.employee");
      $users = $node->selectDb("status='active'","lastname","","",array("id","lastname","firstname"));
      $res = '<select name="viewuser">';
      for ($i=0,$_i=count($users);$i<$_i;$i++)      
        $res .= '<option value="'.$users[$i]["id"].'" '.($this->m_user == $users[$i]["id"] ? 'selected' : '').'>'.$users[$i]["lastname"].', '.$users[$i]["firstname"];
      
      $res .= '</select>';
    }
    return $res;
  }
  
  
  
  function getInitialDate()
  {
    if ($this->m_viewdate!="")
    {
       $initial_date = Array("year"=>substr($this->m_viewdate,0,4),
                             "month"=>substr($this->m_viewdate,5,2),
                             "day"=>substr($this->m_viewdate,8,2));
    }
    else
    {
      $initial_date = Array("year"=>date("Y"),
                            "month"=>date("m"),
                            "day"=>date("d"));
    }
    return $initial_date;
  }
  
  function getLockIcon()
  {
    // Week is locked
    if ($this->m_lock!="")
      return ' <img src="'.atkconfig("atkroot").'atk/images/lock.gif" border="0">';    
  }
  
  function getHourSurveyColumns()
  {
    $defaultcolumns = array("taskdate","userid","phaseid","activity","remark","time","functionlevel");
    $additionalcolumns = atkHarvestModules("getHourSurveyColumns");
    return array_merge($defaultcolumns, $additionalcolumns);
  }
  
  /**
   * This method is called by the hoursurvey to get custom search fields.
   * Derived classes (for example in overloaders) can override this method and
   * Add their own custom search fields.
   */
  function getSelectionItems()
  {
    return array();
  }
  
  /**
   * Get the most recent estimations for each activity. Estimations for activities are stored in the task table.
   * @todo: create a good query that does just that instead of first having to
   * edit the records.      
   *
   * @param type name description
   * @return type description
   */
  function _getMostRecentEstimations()
  {
    $db = &atkGetDb();    
    $query = &new atkQuery();
    
    $query->addTable("task");
    //$query->addField("id AS taskid","","task");
    $query->addField("id AS activityid","","activity");
    $query->addField("id AS phaseid","","phase");
    $query->addField("id AS projectid","","project");    
    $query->addField("current_planning","","task");    
    $query->addJoin("activity","activity","task.refid=activity.id",true);
    $query->addJoin("phase","phase","activity.phaseid=phase.id",true);
    $query->addJoin("project","project","phase.projectid=project.id",true);
    $query->addCondition("task.tasktype='activity'");
    $query->addCondition("project.status='active'");
    $query->addCondition("phase.status='active'");
    $query->addCondition("task.current_planning > 0");
    $query->addOrderBy("task.refid, task.editdatetime ASC");
    $records = $db->getrows($query->buildSelect());    
    $estimations = array();
    
    //we recieve the estimation records ordered by refid and editdatetime.
    //we loop through all records and overwrite every current_planning so in the end we have the most recent one.
    foreach($records as $rec)
    {
      $projectid        = $rec["projectid"];
      $phaseid          = $rec["phaseid"];
      $activityid       = $rec["activityid"];
      $current_planning = $rec["current_planning"];      
      $estimations[$projectid]["phases"][$phaseid]["activities"][$activityid]["current_planning"] = $current_planning;
    }    
    return $estimations;
  } 
}

?>