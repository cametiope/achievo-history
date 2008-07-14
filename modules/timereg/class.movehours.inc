<?php
  atkimport("modules.timereg.hours");
  class movehours extends hours
  {

    function movehours($name="movehours", $flags=0)
    {
      // Use the normal hours node constructor
      $this->hours($name, $flags);

      // Hide anything but the projectid, phaseid and activityid relations
      $attribkeys = array_keys($this->m_attribList);
      for($i=0,$_i=count($attribkeys); $i<$_i; $i++)
        if (!in_array($attribkeys[$i], array("projectid", "phaseid", "activityid", "javascript")))
          $this->m_attribList[$attribkeys[$i]]->addFlag(AF_HIDE);
    }

    function allowed($action, $records=array())
    {
      // Only handle the edit and save action
      if (!in_array($action, array("add","save"))) return false;

      // Make sure we have an array of records
      if (empty($records)) $records = array();
      if (!empty($records) && !isset($records[0])) $records = array($records);

      // Loop trough the records and return false if one of them cannot be moved
      atkImport("modules.utils.dateutil");
      foreach($records as $record)
      {
        $locktype = $this->getLockType($record["userid"]["id"], dateutil::arr2string($record["activitydate"], "Y-m-d"));
        if (!$this->recordIsMoveable($record, $record["userid"]["id"], $locktype))
        {
          return false;
        }
      }
      return true;
    }

    function doMove($ids, $phaseid, $activityid)
    {
      $db = atkGetDb();
      foreach($ids as $id)
      {
        $db->query("UPDATE hours SET phaseid='$phaseid', activityid='$activityid' WHERE hours.id='$id'");
      }
    }

    function action_save()
    {
      // Get the relevant postvars
      $selector = atkArrayNvl($this->m_postvars, "atkselector", array());

      $phase = decodeKeyValuePair(atkArrayNvl($this->m_postvars, "phaseid"));
      $activity = decodeKeyValuePair(atkArrayNvl($this->m_postvars, "activityid"));
      $phaseid = $phase["phase.id"];
      $activityid = $activity["activity.id"];

      // Get a list of hours id's
      $ids = array();
      foreach($selector as $selectoritem)
      {
        $ids[] = intval(atkArrayNvl(decodeKeyValuePair($selectoritem), "hours.id"));
      }

      // Move the hours
      $this->doMove($ids, $phaseid, $activityid);

      // Get a level back
      $this->redirect(session_url(atkSelf(),SESSION_BACK,2));
    }

    function editArray($mode="add", $record=NULL, $forceList="", $suppressList="", $fieldprefix="")
    {
      $result = parent::editArray($mode, $record, $forceList, $suppressList, $fieldprefix);
      $selector = atkArrayNvl($this->m_postvars, "atkselector", array());
      foreach($selector as $selectoritem)
        $result["hide"][] = '<input type="hidden" name="atkselector[]" value="'.$selectoritem.'"/>';
      return $result;
    }


  }

?>
