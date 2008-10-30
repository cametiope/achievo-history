<?php

$config_atkroot = "./../../";
include_once($config_atkroot."atk.inc");
include_once($config_atkroot."achievotools.inc");
atkimport("module.utils.dateutil");

atksession();

$action = atkGetPostVar("action");

//get var from url
$startdate  = fixdate(atkGetPostVar("startdate"));
$enddate  = fixdate(atkGetPostVar("enddate"));
$type = atkGetPostVar("type");
$id = atkGetPostVar("id");
$employeeId = atkGetPostVar('employeeId');
$depth = atkGetPostVar('depth');

/*@var $node projectplanning*/
$node = &atkGetNode("project.projectplanning");

$node->handleDateBounds($startdate, $enddate);
$periods = dateutil::weeksBetween($startdate,$enddate);

switch($type)
{
  case "employee":
    //data collection for current employee
    $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId);
    $node->handleSubproject();

    foreach ($node->m_resourceHours['package'] as $empId=>$packVal)
    {
      foreach ($packVal as $packId=>$v)
      {
        $node->getPackageChild($packId,$empId);

        if(is_array($node->m_parentChild['project.package'][$packId]['package']))
        {
          foreach ($node->m_parentChild['project.package'][$packId]['package'] as $child)
          {
            $node->handleSubPackage($child['id'],$empId, $packId);
          }
        }
      }
    }

    foreach ($node->m_resourceHours['project'][$employeeId] as $projectId=>$r)
    {
      $node->getProjectChild($projectId, $employeeId);
    }

    //iterate of project of current user
    foreach ($node->m_resourceHours['project'][$employeeId] as $projectId=>$r)
    {
      //skip subprojects
      if(!$r['master_project'])
      {
        $line = array();
        $line[] = "&nbsp;";

        $items = $node->getLineproject($periods, $projectId, $employeeId);

        foreach ($items as $item)
        {
          $line[] = atkDurationAttribute::_minutes2string($item);
        }

        $plan = $node->getPlan('project',$projectId,$id);
        $fact = $node->getFact('project',$projectId,$id);

        $line[] = atkDurationAttribute::_minutes2string($plan);
        $line[] = atkDurationAttribute::_minutes2string($fact);
        $line[] = atkDurationAttribute::_minutes2string($plan - $fact);

        $data[] = array("data"=>$line,"id"=>$projectId,"type"=>'project',"name"=>$r['name'],"employeeid"=>$id);
      }
    }
    break;
  case "project":
    //data collection - main project
    $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId, '', $id);

    //get all children of current project where executor is current employee
    $node->getProjectChild($id, $employeeId);

    //data collection - subprojects
    foreach ($node->m_parentChild['project.project'][$id]['project'] as $child)
    {
      $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId, '', $child['id']);
      $node->getProjectChild($child['id'], $employeeId);
    }

    //data collection for sub...
    foreach ($node->m_resourceHours['package'] as $empId=>$packVal)
    {
      foreach ($packVal as $packId=>$v)
      {
        $node->getPackageChild($packId,$empId);

        if(is_array($node->m_parentChild['project.package'][$packId]['package']))
        {
          foreach ($node->m_parentChild['project.package'][$packId]['package'] as $child)
          {
            $node->handleSubPackage($child['id'],$empId, $packId);
          }
        }
      }
    }

    foreach ($node->m_parentChild['project.project'][$id] as $type=>$child)
    {
      //child-package
      foreach ($child as $i)
      {
        $line = array();
        $line[] = "&nbsp;";

        $items = $node->{'getLine'.$type}($periods, $i['id'], $employeeId);

        foreach ($items as $item)
        {
          $line[] = atkDurationAttribute::_minutes2string($item);
        }

        $plan = $node->getPlan($type,$i['id'],$employeeId);
        $fact = $node->getFact($type,$i['id'],$employeeId);

        $line[] = atkDurationAttribute::_minutes2string($plan);
        $line[] = atkDurationAttribute::_minutes2string($fact);
        $line[] = atkDurationAttribute::_minutes2string($plan - $fact);

        $data[] = array("data"=>$line,"id"=>$i['id'],"type"=>$type,"name"=>$i['name'],"employeeid"=>$employeeId);
      }
    }

    break;
  case "package":

    //data collection
    $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId, $id);
    $node->getPackageChild($id,$employeeId);

    if(count($node->m_parentChild['project.package'][$id]['package']))
    {
      foreach ($node->m_parentChild['project.package'][$id]['package'] as $child)
      {
        $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId, $child['id']);
      }
    }

    foreach ($node->m_parentChild['project.package'][$id] as $type=>$child)
    {
      foreach ($child as $i)
      {
        //child-package
        $line = array();
        $line[] = "&nbsp;";

        $items = $node->{'getLine'.$type}($periods, $i['id'], $employeeId);

        foreach ($items as $item)
        {
          $line[] = atkDurationAttribute::_minutes2string($item);
        }

        $plan = $node->getPlan($type,$i['id'],$employeeId);
        $fact = $node->getFact($type,$i['id'],$employeeId);

        $line[] = atkDurationAttribute::_minutes2string($plan);
        $line[] = atkDurationAttribute::_minutes2string($fact);
        $line[] = atkDurationAttribute::_minutes2string($plan - $fact);

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
'padding'=>$depth*20
);

$content = atkinstance("atk.ui.atkui")->render(moduleDir('project').'templates/projectline.tpl',$vars);

echo $content;
exit;

function fixdate($date)
{
  list($year,$month,$day) = explode('-',$date);
  return date("Y-m-d",mktime(0,0,0,$month,$day,$year));
}

?>