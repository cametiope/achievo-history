<?php

  class resourceutils
  {
    static function getWorkingDays($strstart, $strend)
    {
      $ret =0;

      $startstamp = dateUtil::str2stamp($strstart);
      $endstamp   = dateUtil::str2stamp($strend);

      // Calculate the number of days between the selected start and end date
      $nrofdays = floor(($endstamp-$startstamp)/(60*60*24));

      // Loop through the days, starting at the day selected as "from"
      for ($i = 0; $i <= $nrofdays; $i++)
      {
        $weekdate = date("w",$startstamp + (ONEDAY * $i));
        if($weekdate>0  AND $weekdate<6) $ret++;
      }
      return $ret;
    }

    static function str2str($strdate)
    {
      if(strstr($strdate,'-'))
      {
        return $strdate;
      }
      else
      {
        return substr($strdate, 0, 4)."-".substr($strdate, 4, 2)."-".substr($strdate, 6, 2);
      }
    }
    
    static function monthsBetween($datestart, $dateend)
    {
      $ret = array();
      $datestart = dateutil::str2stamp($datestart);
      $dateend = dateutil::str2stamp($dateend);
      
      $start = mktime(0, 0, 0, date("m",$datestart), 1, date("Y", $datestart));
      $end =   mktime(0, 0, 0, date("m",$dateend)+1, 0, date("Y", $dateend));
      
      $i = 0;
      while($start < $end)
      {
        $key = date("y",$start).date("m",$start);
        $tempdate = mktime(0,0,0,date("m",$start)+1,0,date("Y",$start));
        $ret[$key] =array(
                        "month" => $key,
                        "startdate" => date("Ymd",mktime(0,0,0,date("m",$start),1,date("Y",$start))),
                        "enddate" => date("Ymd",$tempdate));
        $i++;
        $start =  mktime(0,0,0,date("m",$tempdate),date("d",$tempdate)+1,date("Y",$tempdate));
      }
      
      return $ret;
    }
  }
?>