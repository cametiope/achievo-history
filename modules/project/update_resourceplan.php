<?php

  $config_atkroot = "./../../";
  include_once($config_atkroot."atk.inc");
  atkimport("module.utils.dateutil");

  atksession();

  $action = atkGetPostVar("action");

  $startdate  = atkGetPostVar("startdate");//present allways
  $enddate  = atkGetPostVar("enddate");//present allways
  $projectid = atkGetPostVar("projectId");//present allways
  $employee = atkGetPostVar('employee');
  $package = atkGetPostVar('package');
  $depth = atkGetPostVar('depth');//present allways

  $startdate = fixdate($startdate);
  $enddate = fixdate($enddate);

  if($employee)
  {
    $node = &atkGetNode("project.resourceplanning");
    $node->m_projectId = $projectid;

    $startweek = date("W",dateutil::str2stamp($startdate));
    $endweek = date("W",dateutil::str2stamp($enddate));

    $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
    $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));

    //data collection
    $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee);

    foreach ($node->m_resourceHours['package'][$employee] as $key=>$r)
    {
      $line = array();
      $line[] = atkDurationAttribute::_minutes2string($node->getPlan('package',$key,$employee));
      $line[] = atkDurationAttribute::_minutes2string($node->getFact('package',$key,$employee));

      for($w=$startweek;$w<=$endweek;$w++)
      {
        $line[] = $w;
      }
      $data[] = array("data"=>$line,"id"=>$key,"type"=>'package',"name"=>$r['name'],"employee"=>$employee);
    }
    $min_width = ($endweek - $startweek +3)*50+190;

    $vars = array('resourceplan'=>$data,
                  'min_width'=>$min_width,
                  'depth'=>$depth,
                  'width'=>190-$depth*20,
                  'padding'=>$depth*20
                    );

    $ui = &atkinstance("atk.ui.atkui");

    $content = $ui->render(moduleDir('project').'templates/resourceline.tpl',$vars);

    echo $content;
    exit;
  }
  elseif($package)
  {
    $employeeId = atkGetPostVar('employeeId');

    $node = &atkGetNode("project.resourceplanning");
    $node->m_projectId = $projectid;

    $startweek = date("W",dateutil::str2stamp($startdate));
    $endweek = date("W",dateutil::str2stamp($enddate));

    $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
    $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));

    //data collection
    $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employeeId, $package);

    foreach ($node->m_parentChild['project.package'][$package] as $r)
    {
      //child-package
      $line = array();
      $line[] = atkDurationAttribute::_minutes2string($node->getPlan($r['type'],$r['id'],$employeeId));
      $line[] = atkDurationAttribute::_minutes2string($node->getFact($r['type'],$r['id'],$employeeId));

      for($w=$startweek;$w<=$endweek;$w++)
      {
        $line[] = $w;
      }
      $data[] = array("data"=>$line,"id"=>$r['id'],"type"=>$r['type'],"name"=>$node->getName($r['type'],$r['id'],$employeeId),"employee"=>$employeeId);
    }

    $min_width = ($endweek - $startweek +3)*50+190;

    $vars = array('resourceplan'=>$data,
                  'min_width'=>$min_width,
                  'depth'=>$depth,
                  'width'=>190-$depth*20,
                  'padding'=>$depth*20
                    );

    $ui = &atkinstance("atk.ui.atkui");

    $content = $ui->render(moduleDir('project').'templates/resourceline.tpl',$vars);

    echo $content;
    exit;

  }

  function fixdate($date)
  {
    list($year,$month,$day) = explode('-',$date);
    return date("Y-m-d",mktime(0,0,0,$month,$day,$year));
  }

?>