<?php

class projecttreeutils
{

  /**
   * Original node type
   *
   * @var atkNodeType
   */
  protected $originalType;

  /**
   * New node type
   *
   * @var atkNodeType
   */
  protected $newType;

  /**
   * Original project or package id
   *
   * @var integer
   */
  protected $originalId;

  /**
   * New  project or package id
   *
   * @var integer
   */
  protected $newId;


  /**
   * Constructor.
   *
   * @param atkNodeType $type
   * @param integer $original
   * @param integer $new
   */
  public function __construct( $originalType = 'project.package', $originalId = 0, $newType = 'project.package', $newId = 0 )
  {
    $this->originalType = $originalType;
    $this->newType = $newType;
    $this->originalId = $originalId;
    $this->newId = $newId;
  }


  /**
   * Update the tree through the bottom-up design.
   *
   */
  public function updateTree($record)
  {
    $db = atkGetDb();
    $securitymanager = &atkGetSecurityManager();
    $initialplanningallowed = $securitymanager->allowed( "project.project", "initialplanning" );

    //update project
    $this->updateProject($record['projectid']['id'], $db, $initialplanningallowed);

    // Recalculate original tree
    $this->calculateHours( $this->originalType, $this->originalId, $db, $initialplanningallowed );

    // Recalculate new tree
    if( $this->newId != 0 )
    {
      $this->calculateHours( $this->newType, $this->newId, $db, $initialplanningallowed );
    }
  }


  /**
   * Calculate the spend hours for packages and projects
   *
   * @param array $record
   */
  protected function calculateHours( $type, $id, $db, $initialplanningallowed)
  {
    if ($type == 'project.package')
    {
      $this->updatePackage($id, $db, $initialplanningallowed);

      list($res) = $db->getrows("SELECT master, master_id FROM package WHERE id='$id'");
      $this->calculateHours($res['master'], $res['master_id'], $db, $initialplanningallowed);
    }
  }

  /**
   * Update the information in packages.
   *
   * Calculate the total spend minutes/hours, current planning hours
   * and remaining effort time hours from underlying packages and phases and
   * update the information in the parent package.
   *
   * @param integer $package_id
   * @param atkDb $db
   */
  protected function updatePackage( $package_id, $db, $initialplanningallowed )
  {
    $select = "
        SELECT"
          .($initialplanningallowed ?
            " (IFNULL((SELECT SUM(initial_planning) FROM phase WHERE master='project.package' AND master_id='$package_id'),0)
             + IFNULL((SELECT SUM(initial_planning) FROM package WHERE master='project.package' AND master_id='$package_id'),0)) AS initial_planning," : "")."

          (IFNULL((SELECT SUM(time) AS spend_minutes FROM hours WHERE phaseid IN (SELECT id FROM phase WHERE master='project.package' AND master_id='$package_id')),0)
           + IFNULL(SUM(p.spend_hours),0)) AS hours,

          (IFNULL((SELECT SUM(current_planning) FROM phase WHERE master='project.package' AND master_id='$package_id'),0)
          +IFNULL((SELECT SUM(current_planning) FROM package WHERE master='project.package' AND master_id='$package_id'),0)) AS current_planning,

          (IFNULL((SELECT SUM(remaining_effort_time) FROM phase WHERE master='project.package' AND master_id='$package_id'),0)
          +IFNULL((SELECT SUM(remaining_effort_time) FROM package WHERE master='project.package' AND master_id='$package_id'),0)) AS remaining_effort_time
        FROM
          package p
        WHERE
          (p.master='project.package' AND p.master_id='$package_id')";

    list($h) = $db->getrows( $select );

    $completed = $this->calculateCompleted( $h['hours'], $h['current_planning'], $h['remaining_effort_time'] );

    $update = "UPDATE
                          package
                        SET"
                        .($initialplanningallowed ? " initial_planning= '".$h['initial_planning']."'," : "")."
                          spend_hours='".$h['hours']."',
                          current_planning='".$h['current_planning']."',
                          completed='$completed',
                          remaining_effort_time='".$h['remaining_effort_time']."'
                        WHERE
                          id='$package_id'";

    $db->query( $update );
  }


  /**
   * Update the information in projects
   *
   * Calculate the total spend minutes/hours, current planning hours
   * and remaining effort time hours from underlying packages and phases and
   * update the information in the parent project.
   *
   * @param integer $project_id
   * @param atkDb $db
   */
  protected function updateProject( $project_id, $db, $initialplanningallowed )
  {
    list($h) = $db->getrows("
        SELECT"
          .($initialplanningallowed ? " SUM(initial_planning) AS initial_planning," : "")."
          SUM(current_planning) AS current_planning,
          SUM(remaining_effort_time) AS remaining_effort_time,
          (SELECT SUM(time) FROM hours WHERE phaseid IN (SELECT id FROM phase WHERE projectid='$project_id')) AS hours
        FROM
          phase
        WHERE
          projectid='$project_id'");

    $completed = $this->calculateCompleted( $h['hours'], $h['current_planning'], $h['remaining_effort_time'] );

    $db->query("UPDATE
                  project
                SET"
                  .($initialplanningallowed ? " initial_planning= '".$h['initial_planning']."'," : "")."
                  spend_hours='".$h['hours']."',
                  current_planning='".$h['current_planning']."',
                  completed='$completed',
                  remaining_effort_time='".$h['remaining_effort_time']."'
                WHERE id='$project_id'");
  }

  /**
   * Calculate the completed value.
   *
   * @param integer $hours
   * @param integer $planning
   * @param integer $left
   * @return integer
   */
  protected function calculateCompleted( $hours, $planning, $left )
  {
    if( $left <= 0 )
      return 100;

    return (round( (( $hours / ($left +$hours)) * 100), 0 ));
  }

  public function updateInitialPlanning($rec, $db)
  {
    list($master,$master_id) = explode("s_",$rec['project_package']);
    $this->setToProject($db,$rec['projectid']['id']);
    $this->setToTree("project.".$master, $master_id, $db, $rec['initial_planning'],$rec['current_planning']);
  }

  private function setToTree($master, $master_id, $db, $initial, $current)
  {
    if($master=="project.package")
    {
      $db->query("
        UPDATE package
        SET initial_planning = initial_planning+'$initial',
            current_planning = current_planning+'$current'
        WHERE id='$master_id'
      ");

      list($res) = $db->getrows("SELECT master, master_id FROM package WHERE id='$master_id'");
      $this->setToTree($res['master'], $res['master_id'], $db, $initial, $current);
    }
  }

  private function setToProject($db, $projectId)
  {
    list($res) = $db->getrows("
      SELECT
        SUM(initial_planning) initial,
        SUM(current_planning) current
      FROM
        phase
      WHERE
        projectid='$projectId'
    ");

    $db->query("
      UPDATE project
      SET initial_planning='".$res['initial']."',
          current_planning='".$res['current']."'
      WHERE id='$projectId'
    ");
  }

}
