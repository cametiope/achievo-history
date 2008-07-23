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

        return substr($strdate, 0, 4)."-".substr($strdate, 5, 2)."-".substr($strdate, 8, 2);
      }
      else
      {
        return substr($strdate, 0, 4)."-".substr($strdate, 4, 2)."-".substr($strdate, 6, 2);
      }

    }
  }
?>