<?php

  $config_atkroot = "./../../";
  include_once($config_atkroot."atk.inc");
  include_once($config_atkroot."achievotools.inc");
  atkimport("module.utils.dateutil");

  atksession();

  $action = atkGetPostVar("action");
  $startdate  = fixdate(atkGetPostVar("startdate"));
  $enddate  = fixdate(atkGetPostVar("enddate"));
  $type = atkGetPostVar("type");
  $id = atkGetPostVar("id");
  $employeeId = atkGetPostVar('employeeId');
  $depth = atkGetPostVar('depth');

  /*@var $node resourceplanning*/
  $node = &atkGetNode("project.resourceplanning");
  $projectid = $node->m_projectId;
  
  $node->handleDateBounds($startdate, $enddate);

  //get period info
  switch($node->getViewMode())
  {
    case "week":
      $periods = dateutil::weeksBetween($startdate,$enddate);
      break;
    case "month":
      $periods = resourceutils::monthsBetween($startdate, $enddate);
      break;
  }

  switch($type)
  {
    case "employee":

      //data collection for current employee
      $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId);

      // We can see package and task (phase) hear, but not subproject
      $node->getProjectChild($projectid, $employeeId);

      //data collection for subpackage
      foreach ($node->m_parentChild['project.project'][$projectid]['package'] as $child)
      {
        $node->handleSubPackage($child['id'],$employeeId);
      }

      foreach ($node->m_parentChild['project.project'][$projectid] as $type=>$child)
      {
        //child-package
        foreach ($child as $i)
        {
          $line = array();
          $line[] = atkDurationAttribute::_minutes2string($node->getPlan($type,$i['id'],$employeeId));
          $line[] = atkDurationAttribute::_minutes2string($node->getFact($type,$i['id'],$employeeId));

          $items = $node->{'getLine'.$type}($periods, $i['id'], $employeeId);

          foreach ($items as $item)
          {
            $line[] = atkDurationAttribute::_minutes2string($item);
          }

          $data[] = array("data"=>$line,"id"=>$i['id'],"type"=>$type,"name"=>$i['name'],"employeeid"=>$employeeId);
        }
      }

      break;
    case "package":
      //data collection for parent and child
      $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId, $id);
      $node->getPackageChild($id, $employeeId);
      
      foreach ($node->m_parentChild['project.package'][$id]['package'] as $child)
      {
        $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId, $child['id']);
      }

      //data collection for subpackage
      foreach ($node->m_resourceHours['package'] as $empId=>$packVal)
      {
        foreach ($packVal as $packId=>$v)
        {
          $node->handleSubPackage($packId, $empId);
        }
      }

      foreach ($node->m_parentChild['project.package'][$id] as $type=>$child)
      {
        foreach ($child as $i)
        {
          $line = array();
          $line[] = atkDurationAttribute::_minutes2string($node->getPlan($type,$i['id'],$employeeId));
          $line[] = atkDurationAttribute::_minutes2string($node->getFact($type,$i['id'],$employeeId));

          $items = $node->{'getLine'.$type}($periods, $i['id'], $employeeId);

          foreach ($items as $item)
          {
            $line[] = atkDurationAttribute::_minutes2string($item);
          }

          $data[] = array("data"=>$line,"id"=>$i['id'],"type"=>$type,"name"=>$i['name'],"employeeid"=>$employeeId);
        }
      }
      break;
  }

  $min_width = $node->getMinWidth(count($periods));

  $vars = array('plan'=>$data,
                'min_width'=>$min_width,
                'depth'=>$depth,
                'width'=>190-$depth*20,
                'padding'=>$depth*20,
                'view'=>$node->getViewMode()
                  );

  $content = atkinstance("atk.ui.atkui")->render(moduleDir('project').'templates/resourceline.tpl',$vars);

  echo $content;
  exit;

  function fixdate($date)
  {
    list($year,$month,$day) = explode('-',$date);
    return date("Y-m-d",mktime(0,0,0,$month,$day,$year));
  }

?>