<?php

  $config_atkroot = "./../../";
  include_once($config_atkroot."atk.inc");
  atkimport("module.utils.dateutil");

  atksession();

  $action = atkGetPostVar("action");

  //get var from url
  $startdate  = atkGetPostVar("startdate");
  $enddate  = atkGetPostVar("enddate");
  $type = atkGetPostVar("type");
  $id = atkGetPostVar("id");
  $employee = atkGetPostVar('employeeId');
  $depth = atkGetPostVar('depth');

  $startdate = fixdate($startdate);
  $enddate = fixdate($enddate);

  $startweek = date("W",dateutil::str2stamp($startdate));
  $endweek = date("W",dateutil::str2stamp($enddate));

  $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
  $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));

  /*@var $node projectplanning*/
  $node = &atkGetNode("project.projectplanning");

  switch($type)
  {
    case "employee":

      //data collection
      $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$id);

      foreach ($node->m_resourceHours['project'][$id] as $key=>$r)
      {
        $node->getProjectChild($key, $id);

        foreach ($node->m_parentChild['project.project'][$key]['project'] as $child)
        {
          $node->handleSubProject($child['id'],$employee);

          $value = $node->getPlan("project",$key,$id) + $node->getPlan("project",$child['id'],$id);
          $node->setPlan("project",$key,$id,$value);

          $value = $node->getFact("project",$key,$id) + $node->getFact("project",$child['id'],$id);
          $node->setFact("project",$key,$id,$value);
        }

        //skip subprojects
        if(!$r['master_project'])
        {
          $line = array();
          $line[] = "&nbsp;";

          for($w=$startweek;$w<=$endweek;$w++)
          {
            $line[] = $w;
          }

          $plan = $node->getPlan('project',$key,$id);
          $fact = $node->getFact('project',$key,$id);

          $line[] = atkDurationAttribute::_minutes2string($plan);
          $line[] = atkDurationAttribute::_minutes2string($fact);
          $line[] = atkDurationAttribute::_minutes2string($plan - $fact);

          $data[] = array("data"=>$line,"id"=>$key,"type"=>'project',"name"=>$r['name'],"employeeid"=>$id);
        }
      }
      break;
    case "project":

      //data collection
      $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee, '', $id);

      $node->getProjectChild($id, $employee);

      foreach ($node->m_parentChild['project.project'][$id]['project'] as $child)
      {
        $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee, '', $child['id']);
      }

      foreach ($node->m_parentChild['project.project'][$id]['package'] as $child)
      {
        $node->handleSubPackage($child['id'],$employee);
      }

      foreach ($node->m_parentChild['project.project'][$id] as $type=>$child)
      {
        //child-package
        foreach ($child as $i)
        {
          $line = array();
          $line[] = "&nbsp;";

          for($w=$startweek;$w<=$endweek;$w++)
          {
            $line[] = $w;
          }

          $plan = $node->getPlan($type,$i['id'],$employee);
          $fact = $node->getFact($type,$i['id'],$employee);

          $line[] = atkDurationAttribute::_minutes2string($plan);
          $line[] = atkDurationAttribute::_minutes2string($fact);
          $line[] = atkDurationAttribute::_minutes2string($plan - $fact);

          $data[] = array("data"=>$line,"id"=>$i['id'],"type"=>$type,"name"=>$i['name'],"employeeid"=>$employee);
        }
      }

      break;
    case "package":

      //data collection
      $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee, $id);
      $node->getPackageChild($id,$employee);

      if(count($node->m_parentChild['project.package'][$id]['package']))
      {
        foreach ($node->m_parentChild['project.package'][$id]['package'] as $child)
        {
          $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee, $child['id']);
        }
      }

      foreach ($node->m_parentChild['project.package'][$id] as $type=>$child)
      {
        foreach ($child as $i)
        {
          //child-package
          $line = array();
          $line[] = "&nbsp;";

          for($w=$startweek;$w<=$endweek;$w++)
          {
            $line[] = $w;
          }

          $plan = $node->getPlan($type,$i['id'],$employee);
          $fact = $node->getFact($type,$i['id'],$employee);

          $line[] = atkDurationAttribute::_minutes2string($plan);
          $line[] = atkDurationAttribute::_minutes2string($fact);
          $line[] = atkDurationAttribute::_minutes2string($plan - $fact);

          $data[] = array("data"=>$line,"id"=>$i['id'],"type"=>$type,"name"=>$i['name'],"employeeid"=>$employee);
        }
      }
      break;
  }

  $min_width = ($endweek - $startweek +3)*50+190;

  $vars = array('plan'=>$data,
                'min_width'=>$min_width,
                'depth'=>$depth,
                'width'=>190-$depth*20,
                'padding'=>$depth*20
                  );

  $ui = &atkinstance("atk.ui.atkui");
  $content = $ui->render(moduleDir('project').'templates/projectline.tpl',$vars);

  echo $content;
  exit;

  function fixdate($date)
  {
    list($year,$month,$day) = explode('-',$date);
    return date("Y-m-d",mktime(0,0,0,$month,$day,$year));
  }

?>