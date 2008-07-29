<?php
  useattrib("atkattribute");
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atkdummyattribute");
  useattrib("project.projectmanytoonerelation");

  userelation("atkmanytoonerelation");

  atkimport("module.project.utils.projectutils");
  atkimport("module.timereg.hours");

  class timereg_advanced extends atkNode
  {

  	/**
  	 * Constructor.
  	 *
  	 * @param string $name
  	 * @param integer $flags
  	 */
    public function __construct($name="timereg_advanced", $flags=0)
    {
      $this->atkNode($name);

      $this->add(new atkNumberAttribute("id",AF_AUTOKEY));

      $this->add(new atkDateAttribute("activitydate","","", 0, (atkconfig("timereg_allowfuture", false)?0:date("Ymd")), AF_OBLIGATORY|AF_FORCE_LOAD|($this->m_weekview?0:AF_HIDE_LIST)),null,10);
      //$this->add(new to_date());
      $this->add(new atkDummyAttribute("dayfilter", "", AF_HIDE), null, 11);

      // force_load flag is set for userid, since several features rely on the
      // userid, even if it's not displayed on screen.
      $this->add(new atkManyToOneRelation("userid","employee.employee", AF_HIDE|AF_FORCE_LOAD|AF_OBLIGATORY));

      //phaseid must be added before the project id to make sure the join works
      //we need the projectid to sort on
      $phase = &new atkManyToOneRelation("phaseid","project.phaseselector",AF_OBLIGATORY|AF_FORCE_LOAD);
      $phase->setDestinationFilter("phase.projectid='[projectid.id]'");
      $oneActivity = (projectutils::getActivityCount() == 1);
      if(!$oneActivity)
        $phase->addDependee("activityid");
      $this->add($phase,null, 30);

      $project = &new projectManyToOneRelation("projectid","project.project",AF_OBLIGATORY|AF_FORCE_LOAD);
      $project->addDependee("phaseid");
      $project->setUserId($this->m_user);

      if(atkconfig::get("timereg","timereg_contact_link",false))
      {
        $project->addDependee("contact_id");
      }

      $this->add($project,null,20);

      if($oneActivity)
        $activity_flag = AF_HIDE|AF_FORCE_LOAD;
      else
        $activity_flag = AF_LIST_OBLIGATORY_NULL_ITEM;
      $activity = &new atkManyToOneRelation("activityid","project.activity",AF_OBLIGATORY|$activity_flag);
      $this->add($activity);

      if(atkconfig::get("timereg","timereg_contact_link",false))
      {
        $this->add(new atkManyToOneRelation("contact_id","organization.contact"));
      }

      $this->add(new atkTextAttribute("remark", atkconfig("timereg_remark_lines", 1), 0));
      $this->add(new atkDurationAttribute("time",atkconfig("timereg_resolution", "15m"), atkconfig("max_bookable",10).'h'));

      // If we don't use duration, then we won't need this
      // Because we can set the rates with the billing module
      if (atkconfig("use_duration", true))
      {
        $relation = &new atkManyToOneRelation("workperiod","timereg.workperiod", AF_HIDE_LIST|AF_RELATION_NO_NULL_ITEM);
        $relation->setHideWhenEmpty(true);
        $this->add($relation);
      }

      $this->add(new atkDateAttribute("entrydate",AF_HIDE|AF_OBLIGATORY));
      $this->setTable("hours","hours");
    }


    public function action_add()
    {
      $page = &$this->getPage();
      $ui = &$this->getUi();
      $smarty = &atkinstance('atk.ui.atksmarty');
      $theme = &atkTheme::getInstance();

      $rows = $this->getActivePhases();
      $smarty->assign('rows', $rows);

      $content = $smarty->fetch(moduleDir('timereg_advanced').'templates/timereg.tpl');

      $page->register_style($theme->stylePath("style.css"));
      $page->register_style($theme->stylePath("tabs.css"));
      $page->register_style($theme->stylePath("recordlist.css"));
      $page->register_script("./atk/javascript/recordlist.js");

      $output = $ui->renderBox(array("title"=>atktext('advanced_time_registration'),
                                   "content"=>$content));

      $page->addContent($this->renderActionPage("showhelloworld", $output));
    }


    private function getActivePhases()
    {
      $node = atkGetNode('project.phase');
      $rows = $node->select($node->getTable().'.startdate < NOW() AND '.$node->getTable().'.enddate > NOW()')
                   ->allRows();
      return $rows;
    }
  }