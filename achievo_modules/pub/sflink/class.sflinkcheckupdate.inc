<?php

include_once (moduleDir("reports")."class.basereport.inc");

useattrib("atkdatetimeattribute");
useattrib("atklistattribute");

class sflinkcheckupdate extends basereport
{
  
  public function __construct()
  {
    parent::basereport("sflinkcheckupdate");

    $this->add(new atkListAttribute("node",array("Account","Contact"),"",AF_LIST_NO_NULL_ITEM));
    $this->add(new atkDateTimeAttribute("from"));
    $this->add(new atkDateTimeAttribute("to"));
  }

  public function initial_values()
  {
    $node = &atkGetNode("sflink.sflink");

    //Set current time to get records from Salesforce
    $endTime = mktime();
    //get last update time from log
    $startTime = $node->getLastUpdateTime();

    return array("from"=>atkDateTimeAttribute::datetimeArray(date("YmdHis",$startTime)),"to"=>atkDateTimeAttribute::datetimeArray(date("YmdHis",$endTime)));
  }
  
  public function getContentElements($filterrecord)
  {
    $node = &atkGetNode("sflink.sflink");
    if($filterrecord['node']=='Account')
    {
      $objName = "organization.organization";      
    }
    else 
    {
      $objName = "organization.contact";      
    }
    
    $object = &atkGetNode($objName);
     
    $selector = $object->getTable().".".$node->buildSelector($filterrecord);
    if($selector)
    {
      $columnConfig = &$this->getColumnConfig();
      $orderby = atkArrayNvl($this->m_postvars,'atkorderby');

      /*@var $columnConfig atkColumnConfig*/
      if ($orderby)
      {
        $columnConfig->clearOrder();
        list($order,$direction) = explode(" ",$orderby);
        if(empty($direction))
        {
          $direction = "asc";
        }
        
        $columnConfig->setSortDirection($order, $direction);
        $columnConfig->setSortOrder($order, 1);
        $orderby = $columnConfig->getOrderByStatement();
      }
      

      $object->getAttribute("salesforce_id")->removeFlag(AF_HIDE_LIST);
      $object->getAttribute("id")->removeFlag(AF_HIDE_LIST);
      
      $data = $object->selectDb($selector,$orderby);

      $actions = array();
      $actions["edit"] = dispatch_url($objName,"edit",array("atkselector"=>"[pk]"));

      /*@var $rl atkrecordlist*/
      $rl = &atknew("atk.recordlist.atkrecordlist");
      $content = $rl->render($object, $data, $actions,RL_NO_SEARCH|RL_NO_SORT);
  
      if(count($data)==0) return array("sflinkcheckupdate"=>$this->text("empty_resultset"));
  
      return array("sflinkcheckupdate"=>$content);
    }
  }

}