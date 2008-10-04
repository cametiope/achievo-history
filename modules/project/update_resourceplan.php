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
  $node->handleDateBound($startdate, $enddate, $startweek, $endweek);

  //get weeks info
  $weeks = dateutil::weeksBetween($startdate,$enddate);

  switch($type)
  {
    case "employee":

      //data collection
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

          $items = $node->{'getLine'.$type}($weeks, $i['id'], $employeeId);

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
      
      //data collection for subpackage
      foreach ($node->m_parentChild['project.package'][$id]['package'] as $child)
      {
        $node->handleSubPackage($child['id'],$employeeId);
      }

      foreach ($node->m_parentChild['project.package'][$id]['package'] as $child)
      {
        $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId, $child['id']);
      }

      foreach ($node->m_parentChild['project.package'][$id] as $type=>$child)
      {
        foreach ($child as $i)
        {
          $line = array();
          $line[] = atkDurationAttribute::_minutes2string($node->getPlan($type,$i['id'],$employeeId));
          $line[] = atkDurationAttribute::_minutes2string($node->getFact($type,$i['id'],$employeeId));

          $items = $node->{'getLine'.$type}($weeks, $i['id'], $employeeId);

          foreach ($items as $item)
          {
            $line[] = atkDurationAttribute::_minutes2string($item);
          }

          $data[] = array("data"=>$line,"id"=>$i['id'],"type"=>$type,"name"=>$i['name'],"employeeid"=>$employeeId);
        }
      }
      break;
  }

  $min_width = ($endweek - $startweek +3)*50+190;

  $vars = array('plan'=>$data,
                'min_width'=>$min_width,
                'depth'=>$depth,
                'width'=>190-$depth*20,
                'padding'=>$depth*20,
                'view'=>$node->getView()
                  );

  $ui = &atkinstance("atk.ui.atkui");
  $content = $ui->render(moduleDir('project').'templates/resourceline.tpl',$vars);

  echo $content;
  exit;

  function fixdate($date)
  {
    list($year,$month,$day) = explode('-',$date);
    return date("Y-m-d",mktime(0,0,0,$month,$day,$year));
  }

?>