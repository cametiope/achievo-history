<?php

include_once (moduleDir("reports")."class.basereport.inc");

userelation("atkmanytoonerelation");

useattrib("atkdateattribute");
useattrib("atkboolattribute");
useattrib("atklistattribute");
useattrib("atkdummyattribute");

class projectsurvey extends basereport
{
  public function __construct()
  {
    parent::basereport("projectsurvey");

    /**
     * Project - autocomplete field with a 'select' button to select from a list
     * In the autocomplete, I should be able to type a project name or a project number
     */
    $this->add(new atkManyToOneRelation("project","project.project", AF_MANYTOONE_AUTOCOMPLETE))
    ->setAutoCompleteSearchFields(array("name","abbreviation"));

    $this->add(new atkBoolAttribute("include_subprojects"));

    /**
     * from/to dates; if set, will only show projects/phases that
     * have been worked on in that time frame.
     *
     * Ivo-"I want to select projects that have been worked on last week for
     * example, but then I still want to see their total numbers so I can know if it's
     * on track".
     */
    $this->add(new atkDummyAttribute("d1",$this->text("date_filter_explain")));//AF_NO_LABEL - see ticket #1391 in external bugzilla
    $this->add(new atkDateAttribute("from"));
    $this->add(new atkDateAttribute("to"));

    /**
     * select only projects/phases from this coordinator
     */
    $this->add(new atkManyToOneRelation("coordinator","employee.employee"))
      ->addDestinationFilter("person.status='active'")
      ->setNoneLabel($this->text("all"));

    /**
     * select only projects/phases that this person has booked time on
     */
    $this->add(new atkManyToOneRelation("worked_on_by","employee.employee"))
      ->addDestinationFilter("person.status='active'")
      ->setNoneLabel($this->text("all"));

    /**
     * select projects that match this description or phases that match
     * this description
     *
     * in any case our data grouping on level project and may be on level phase.
     * So I think, that we can't hide project field, and when we found phase with appropriate description
     * (see this attribute) - we have to show data for project too ????
     */
    $this->add(new atkAttribute("description",0,50));

    /**
     * We want to have default only active projects/phases but
     * in the search criteria make it possible to search in only closed projects or both
     */
    $this->add(new atkListAttribute("projectstatus",array("active","nonactive","archived","all"),"",AF_LIST_NO_NULL_ITEM));
    $this->add(new atkListAttribute("phasestatus",array("active","nonactive","all"),"",AF_LIST_NO_NULL_ITEM));

    /**
     * ability to turn on/off columns (similar to hoursurvey)
     */
    $this->add(new atkMultiSelectAttribute("show_only_columns",
    array($this->text("project"), $this->text("phase"), $this->text("coordinator"),
    $this->text("startdate"), $this->text("enddate"), $this->text("first_hr"), $this->text("last_hr"),
    $this->text("initial_planning"), $this->text("current_planning"), $this->text("spend_hours")),
    array("projectid", "name", "coordinator","startdate", "enddate", "first_hr", "last_hr",
    "initial_planning", "current_planning", "spend_hours"),4));
  }

  /**
   * This function take into account all filter
   * and build selector
   *
   * @param array $filterrecord
   * @param atkNode $dest
   * @return unknown
   */
  private function buildProjectSelector($filterrecord)
  {
    $projectfilter = $hoursfilter = array();

    $node = &atkGetNode("project.project");
    if(!$node->allowed("any_project"))
    {
      $projectfilter[] = "project.coordinator='".atkGetUserId()."'";
    }

    if(isset($filterrecord['project']['id']))
    {
      if(atkArrayNvl($filterrecord,'include_subprojects'))
      {
        atkimport('modules.reports.utils.reportutils');
        $ids = reportutils::getProjectAndSubProjectId($filterrecord['project']['id']);
        $condition = "IN(".implode(",",$ids).")";
        $projectfilter[] = "project.id $condition";
      }
      else
      {
        $projectfilter[] = "project.id = '".$filterrecord['project']['id']."'";
      }
    }

    if(!empty($filterrecord['from']))
    {
      $from = dateutil::arr2str($filterrecord["from"],"Y-m-d");
      $hoursfilter[] = "h.activitydate >= '$from'";
    }

    if(!empty($filterrecord['to']))
    {
      $to = dateutil::arr2str($filterrecord["to"],"Y-m-d");
      $hoursfilter[] = "h.activitydate <= '$to'";
    }

    if(isset($filterrecord['worked_on_by']['id']))
    {
      $hoursfilter[] = "h.userid='".$filterrecord['worked_on_by']['id']."'";
    }

    if(!empty($hoursfilter))
    {
      $hoursfilterstr = implode(" AND ",$hoursfilter);
      $projectfilter[] = "
      project.id IN(
        SELECT
          p.projectid
        FROM hoursbase h JOIN phase p ON h.phaseid=p.id
        WHERE $hoursfilterstr
        GROUP BY p.projectid)";
    }

    if(isset($filterrecord['coordinator']['id']))
    {
      $projectfilter[] = "project.coordinator = '".$filterrecord['coordinator']['id']."'";
    }

    if(!empty($filterrecord['description']))
    {
      $projectfilter[] = "(LOWER(project.description) LIKE LOWER('%".$filterrecord['description']."%')
        OR LOWER(phase.description) LIKE LOWER('%".$filterrecord['description']."%'))";
    }

    if($filterrecord['projectstatus']<>'all')
    {
      $projectfilter[] = "project.status = '".$filterrecord['projectstatus']."'";
    }

    if($filterrecord['phasestatus']<>'all')
    {
      $projectfilter[] = "phase.status = '".$filterrecord['phasestatus']."'";
    }

    $where = implode(" AND ",$projectfilter);
    if(!empty($where)) $where = "WHERE ".$where;

    $projectid_list = array();
    $phaseid_list = array();

    $db = &$this->getDb();
    $rows = $db->getrows("
      SELECT
        project.id projectid,
        phase.id phaseid
      FROM
        project JOIN phase ON project.id=phase.projectid
      $where");

    foreach ($rows as $row)
    {
      $projectid_list[] = $row['projectid'];
      $phaseid_list[] = $row['phaseid'];
    }

    $where = array();
    $ret = "WHERE false";
    if(!empty($projectid_list))
    {
      $where[] = "p.id IN(" . implode(",",array_unique($projectid_list)) . ")";
    }
    if(!empty($phaseid_list)) $where[] = " t.id IN(" . implode(",",array_unique($phaseid_list)) . ")";

    if(!empty($where)) $ret = " WHERE ". implode(" AND ", $where);

    return $ret;
  }

  public function getContentElements($filterrecord)
  {
    $initial = $this->initial_values();
    $supress = array_diff($initial['show_only_columns'],$filterrecord['show_only_columns']);

    /**
     * If I choose to hide the phase column, then it
     * needs to just show 1 line per project, and the initial/current/actual should be
     * the sum of the phases. I.o.w. if I have a 3-phase project, each with 50 hours,
     * and I'm asking for a report on all my current projects but not at the phase
     * level, it should just show the project with 150 as planning.
     *
     * "name" - phase field here.
     */
    if(!in_array("name",$filterrecord['show_only_columns']))
    {
      sort($supress);
      array_splice($supress,array_search("name",$supress),1);

      $key = array_search("projectid",$supress);
      if($key!==FALSE) $supress[$key] = 'name';

      $node = &atkGetNode("reports.surveyproject");
    }
    else
    {
      $node = &atkGetNode("reports.surveyphase");
    }

    $selector = $this->buildProjectSelector($filterrecord);
    $node->m_filterrecord = $selector;

    $columnConfig = &$node->getColumnConfig();
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

    $data = $node->selectDb("",$orderby);
    if($filterrecord['report_output_type'] == RT_EXPORT)
    {
      $rl = &atknew("atk.recordlist.atkcsvrecordlist");
      $rl->render($node, $data, "", $suppress);
    }

    /*@var $rl atkrecordlist*/
    $rl = &atknew("atk.recordlist.atkrecordlist");
    $content = $rl->render($node, $data,array(),RL_NO_SEARCH,$supress);

    if(count($data)==0) return array("projectsurvey_report"=>$this->text("empty_resultset"));

    return array("projectsurvey_report"=>$content);
  }

  public function initial_values()
  {
    return array("show_only_columns"=>array("projectid", "name", "coordinator","startdate", "enddate", "first_hr", "last_hr",
    "initial_planning", "current_planning", "spend_hours"),"projectstatus"=>"active","phasestatus"=>"active","subproject"=>0);
  }

  public function project_selection($record, $mode)
  {
    /**
     * security: you can only select projects that you are a coordinator of
     * unless you have 'all projects' privilege. (I may change my mind later, if it's useful
     * for developers I might want to change this).
     */
    $node = &$this->getAttribute("project")->getDestination();
    if(!$node->allowed("any_project"))
    {
      $node->addFilter("project.coordinator='".atkGetUserId()."'");
    }
    return $this->getAttribute("project")->getSelectableRecords($record, $mode);
  }


}