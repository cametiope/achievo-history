<?php

  $config_atkroot = "./../../";
  include_once($config_atkroot."atk.inc");
  atkimport("module.utils.dateutil");

  atksession();

  $action = atkGetPostVar("action");

  $startdate  = atkGetPostVar("startdate");//present allways
  $enddate  = atkGetPostVar("enddate");//present allways
  $type = atkGetPostVar("type");
  $id = atkGetPostVar("id");
  $employee = atkGetPostVar('employeeId');//present allways
  $depth = atkGetPostVar('depth');//present allways

  $startdate = fixdate($startdate);
  $enddate = fixdate($enddate);

  switch($type)
  {
    case "employee":

      /*@var $node projectplanning*/
      $node = &atkGetNode("project.projectplanning");
      $node->m_projectId = $projectid;

      $startweek = date("W",dateutil::str2stamp($startdate));
      $endweek = date("W",dateutil::str2stamp($enddate));

      $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
      $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));

      //data collection
      $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$id);

      foreach ($node->m_resourceHours['project'][$id] as $key=>$r)
      {
        $line = array();
        $line[] = atkDurationAttribute::_minutes2string($node->getPlan('project',$key,$id));
        $line[] = atkDurationAttribute::_minutes2string($node->getFact('project',$key,$id));

        for($w=$startweek;$w<=$endweek;$w++)
        {
          $line[] = $w;
        }
        $data[] = array("data"=>$line,"id"=>$key,"type"=>'project',"name"=>$r['name'],"employeeid"=>$id);
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
      break;
    case "project":
      $node = &atkGetNode("project.projectplanning");

      $startweek = date("W",dateutil::str2stamp($startdate));
      $endweek = date("W",dateutil::str2stamp($enddate));

      $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
      $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));

      //data collection
      $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee, '', $id);

      $node->getProjectChild($id, $employee);

      if(count($node->m_parentChild['project.project'][$id]['project']))
      {
        foreach ($node->m_parentChild['project.project'][$id]['project'] as $child)
        {
          $node->getTaskHours(resourceutils::str2str($startdate), resourceutils::str2str($enddate),$employee, '', $child_id['id']);
        }
      }

      foreach ($node->m_parentChild['project.project'][$id] as $type=>$child)
      {
        //child-package
        foreach ($child as $i)
        {
          $line = array();
          $line[] = atkDurationAttribute::_minutes2string($node->getPlan($type,$i['id'],$employee));
          $line[] = atkDurationAttribute::_minutes2string($node->getFact($type,$i['id'],$employee));

          for($w=$startweek;$w<=$endweek;$w++)
          {
            $line[] = $w;
          }
          $data[] = array("data"=>$line,"id"=>$i['id'],"type"=>$type,"name"=>$i['name'],"employeeid"=>$employee);
        }
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
      break;
    case "package":
      $node = &atkGetNode("project.projectplanning");

      $startweek = date("W",dateutil::str2stamp($startdate));
      $endweek = date("W",dateutil::str2stamp($enddate));

      $startdate = dateutil::startOfWeek(dateutil::arr2str((dateutil::str2arr($startdate))));
      $enddate = dateutil::endOfWeek(dateutil::arr2str((dateutil::str2arr($enddate))));

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
          $line[] = atkDurationAttribute::_minutes2string($node->getPlan($type,$i['id'],$employee));
          $line[] = atkDurationAttribute::_minutes2string($node->getFact($type,$i['id'],$employee));

          for($w=$startweek;$w<=$endweek;$w++)
          {
            $line[] = $w;
          }
          $data[] = array("data"=>$line,"id"=>$i['id'],"type"=>$type,"name"=>$i['name'],"employeeid"=>$employee);
        }
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
      break;
  }

  function fixdate($date)
  {
    list($year,$month,$day) = explode('-',$date);
    return date("Y-m-d",mktime(0,0,0,$month,$day,$year));
  }

?>