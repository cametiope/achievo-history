<?php

class teamcapacity 
{ 
  
  function teamcapacity()
  {
    
  }
  
  /**
   * Adds capacity information about each team to the array.      
   *
   * @param type name description
   * @param type name description
   * @return type description
   */
  function getCapacity(&$period)
  {
    /*first we get all the data we need.*/
    $teams      = $this->_getTeams();
    
    $contracts = array();
    foreach($teams as $team)
    {
      //get userids from this team.
      $userids                            = $this->_getIdsFromTeamMembers($team["teammembers"]);      
      $contracts[$team["id"]]             = $this->_getUserContracts($period, $userids);
      $capacity[$team["id"]]["weekspeed"] = $this->_getTeamWeekSpeeds($period, $userids);
      $capacity[$team["id"]]["name"]      = $team["name"];
      $capacity[$team["id"]]["id"]        = $team["id"];
    }    
    return $capacity;
  }
  
  /**
   * Get all teams that have members
   * 
   * @return array teams
   */
  function _getTeams()
  { 
    /* @var $node atkNode */
    $node = &atkGetNode("employee.team");      
    $records = $node->selectDb();    
    $teams = array();
    foreach($records as $rec)
    {
      if(count($rec["teammembers"]))
        $teams[] = $rec;
    }    
    return $teams;
  }
  
  
  function _getIdsFromTeamMembers($records)
  {
    $ids = array();
    foreach ($records as $rec)
    {      
      if(isset($rec["employeeid"]["id"]))
        $ids[] = $rec["employeeid"]["id"];
    }    
    return $ids;
  }
  /**
   * Retrieve only the id values from an array of records.     
   *
   * @param array $records records with data
   * @return array ids
   */
  function _getIdsFromRecords($records)
  {
    $ids = array();
    foreach ($records as $rec)
    {   
      if(isset($rec["id"]))
        $ids[] = $rec["id"];
    }
   
    return $ids;
  }
  
  function _getUserContracts($period,$userids)
  {
    /* @var $node atkNode */
    $node = &atkGetNode("employee.usercontracts");
    $fromdate = $period["from"];
    $todate = $period["to"];
    
    //filter to only select contracts that are totally or partly in the period we are interested in.
    $filter = "'$fromdate' < usercontract.enddate AND '$todate' > usercontract.startdate AND usercontract.userid IN (".implode(",",$userids).")";
    return $node->selectDb($filter);    
  }
  
  function _getTeamWeekSpeeds(&$period,&$userids)
  {
    $weekspeeds = array();
    
    //get all data first
    $speeds         = $this->_getEmployeesSpeed($userids);    
    //get calendar hours
    $calendarhours  = $this->_getCalendarHours($period,$userids,$speeds);
    
    //atk_var_dump($calendarhours,"<h1>calendarhours of ".implode(",",$userids)."</h1>");
    //atk_var_dump($period["weeks"],"<h1>weeks</h1>");
    //atk_var_dump($calendarhours,"<h1>calendarhours of ".implode(",",$userids)."</h1>");
    //calculate the weekspeed for each week
    foreach($period["weeks"] as $week)
    {
      $weekspeeds[$week["week"]] = $speeds["totalweekbasespeed"];
      if(isset($calendarhours[$week["week"]]["total"]))
      {
        $weekspeeds[$week["week"]] = $weekspeeds[$week["week"]] - $calendarhours[$week["week"]]["total"];
      }      
    }
    
    return $weekspeeds;
  }
  
  /**
   * Get base speeds for a group of users
   *
   * @param type name description
   * @return type description
   */
  function _getEmployeesSpeed($userids)
  {
    $basespeeds = array();
    $totalspeed = 0;
    foreach ($userids as $id)
    {
      //dummy, assume 40 hours a week.
      $daysaweek = 5;
      $contracthours = 40;
      $percentage = 100;     //100%
      $basespeeds[$id]["contracthours"] = $contracthours;
      $basespeeds[$id]["percentage"]    = $percentage;
      $basespeeds[$id]["daysofweek"]    = "1|2|3|4|5";
      $basespeeds[$id]["weekbasespeed"] = $contracthours/100 * $percentage;
      $basespeeds[$id]["daybasespeed"]  = $basespeeds[$id]["weekbasespeed"]/$daysaweek;
      $totalspeed += $basespeeds[$id]["weekbasespeed"];
    }
    $basespeeds["totalweekbasespeed"] = $totalspeed;
    return $basespeeds;
  }
  
  function _getCalendarHours(&$period, &$userids, &$speeds)
  {
    $node     = &atkGetNode("calendar.schedule");
    $typenode = &atkGetNode("calendar.schedule_types");
    $fromdate = $period["from"];
    $todate   = $period["to"];
    
    $db     = &atkGetDb();
    $query  = &new atkQuery();
    
    /* @var $query atkQuery */
    $query->addField("id","","schedule","schedule_",true);
    $query->addField("person_id","","schedule_attendee","",true);
    //temp
    $query->addField("title","","schedule","",true);
    $query->addField("startdate","","schedule","",true);    
    $query->addField("enddate","","schedule","",true);
    $query->addField("starttime","","schedule","",true);
    $query->addField("endtime","","schedule","",true);
    $query->addField("allday","","schedule","",true);
    $query->addTable("schedule_attendee");
    $query->addJoin("schedule","","schedule_attendee.schedule_id=schedule.id",true);
    $query->addJoin("schedule_type","","schedule.scheduletype=schedule_type.id",true);
    $query->addCondition("'$fromdate' < schedule.enddate AND '$todate' > schedule.startdate");
    $query->addCondition("schedule_attendee.person_id IN (".implode(",",$userids).")");
    $query->addCondition("schedule_type.reducescapacity = 1");
    
    /* @var $node atkNode */
    $records  = $db->getrows($query->buildSelect());
    
    //atk_var_dump($records,"<h1>cal records</h1>");
    //atk_var_dump($period["weeks"],"<h1>weeks</h1>");
    
    $hours = array();    
    
    foreach ($period["weeks"] as $week)
    {
      //search calendar items which are within this week
      foreach($records as $item)
      {
        //get hours for this item for this week.        
        $weekhours = $this->_getThisWeeksHoursOfCalendarItem($item,$week,$speeds[$item["person_id"]]);
        
        if($weekhours != 0)
        {
          //init if needed
          if(!isset($hours[$week["week"]]["users"][$item["person_id"]]))  $hours[$week["week"]]["users"][$item["person_id"]] = 0;
          if(!isset($hours[$week["week"]]["total"]))                      $hours[$week["week"]]["total"] = 0;
          
          //calculate hours
          $hours[$week["week"]]["users"][$item["person_id"]] += $weekhours;
          $hours[$week["week"]]["total"] += $weekhours;
        }
      }
    }
    
    return $hours;    
  }
  
  
  /**
   * calculate the exact number of hours.
   *   
   * @param type name description
   * @return type description
   */
  function _getThisWeeksHoursOfCalendarItem(&$item, &$week, &$speed)
  {
    $startweekstamp = dateutil::str2stamp($week["startdate"]);
    $endweekstamp   = dateutil::str2stamp($week["enddate"]);
    
    $startitemstamp = dateutil::str2stamp(substr($item["startdate"],0,4).substr($item["startdate"],5,2).substr($item["startdate"],8,2));
    $enditemstamp = dateutil::str2stamp(substr($item["enddate"],0,4).substr($item["enddate"],5,2).substr($item["enddate"],8,2));
        
    //is this calendar item in this week?
    if($startitemstamp < $endweekstamp && $enditemstamp > $startweekstamp)
    { 
      //atkdebug("this week");
      //does this calendar item span multiple days?
      if($startitemstamp == $enditemstamp)                                    //NO
      {        
        //atkdebug("one day");
        //is this an all day item? yes -> get basespeed hours of one day
        if($item["allday"] == 1)
        {          
          //atkdebug("all day");          
          return $speed["daybasespeed"];
        }
        else 
        {
          //@todo: basespeed time should be the max
          atkdebug("not all day");
          //calculate the hours (starttime - endtime)          
          return $this->hours_diff($item["endtime"],$item["starttime"]);
        }
      }
      else                                                                    //YES
      {
        atkdebug("multiple days");
          
        //calculate how many days in this week this calender item is valid
        $matchingdays = 0;        
        
        $days = $this->daysBetween($week["startdate"],$week["enddate"]);        
        foreach($days as $day)
        {
          //if startdate of cal item is on or before this day AND enddate is on or after this day --> goodday
          if(dateutil::date_diff($day,$item["startdate"])>=0 && dateUtil::date_diff($day,$item["enddate"])<=0)
            $matchingdays++;          
        }
        
        if($item["allday"] == 1)        
          return $speed["daybasespeed"] * $matchingdays;        
        else 
        {
          //@todo: basespeed time should be the max
          //calculate the hours (starttime - endtime)          
          //return hours + minutepart as a percentage of hours TIMES the number of matching days.
          return $this->hours_diff($item["endtime"],$item["starttime"]) * $matchingdays;
        }
      }
    }
    else
      atkdebug("not this week, so ignore this calendar item.");    
    
    return 0;
  }   
  
  function hours_diff($time2,$time1)
  {
    $endtime    = explode(":",$time2);
    $starttime  = explode(":",$time1);
    
    $hoursdiff = $endtime[0] - $starttime[0];    
    
    if($endtime[1]<$starttime[1])
    {
      $hoursdiff--;
      $endtime[1] += 60;
    }
    
    $minutesdiff = $endtime[1] - $starttime[1];
    
    //return hours + minutepart as a percentage of hours.
    return $hoursdiff + ($minutesdiff/60);
  }
  
  function daysBetween($strstart, $strend)
  {      
    // Initialize the weekdata array
    $daydata = array();
    $oneday = 86400;  //24*60*60
    
    // First fill the weeks and start/end dates..
    $startstamp = dateUtil::str2stamp($strstart);
    $endstamp   = dateUtil::str2stamp($strend);

    // Calculate the number of dates between the selected start and end date
    $nrofdays = dateUtil::date_diff($strstart, $strend);

    // Loop through the days, starting at the day selected as "from"
    for ($i = 0; $i <= $nrofdays; $i++)
    {
      // Calculate the current day depending on the startdate and iterator
      $curstamp = $startstamp + ($oneday * $i);

      // If the day is a monday, then register the weekid, startdate and
      // enddate of the week in the weekdata array and jump one week ahead
      $key = date("Ymd",$curstamp);
      $daydata[$key]["day"] = $key;
      $daydata[$key]["date"] = date("Ymd",$curstamp);
    }

    // Return the array containing the dayid, and date for each day within the given from and to dates     
    return $daydata;    
  }

}



?>