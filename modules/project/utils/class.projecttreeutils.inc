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
    $this->originalType = $this->getNodeFromType($originalType);
    $this->newType = $this->getNodeFromType($newType);
    $this->originalId = $originalId;
    $this->newId = $newId;
  }


  /**
   * Update the tree through the bottom-up design.
   *
   */
  public function updateTree()
  {
    // Recalculate original tree
    $this->calculateHours( $this->originalType, $this->originalId );
    
    // Recalculate new tree
    if( $this->newId != 0 )
    {
      $this->calculateHours( $this->newType, $this->newId, $this->originalType, $this->originalId );
    }
  
  }


  /**
   * Calculate the spend hours for packages and projects
   *
   * @param array $record
   */
  protected function calculateHours( $type, $id )
  {
    $db = atkGetDb();
    
    if ($type == 'package')
    {
      $select = sprintf( "SELECT id, master, master_id FROM %s WHERE id=%d LIMIT 0,1", $type, $id );
      $rows = $db->getrows( $select );
      $this->setUpdater( $rows[0], $db );
    }
    else 
    {
      $this->updateProject($id, $db);
    }
  }


  /**
   * Call custom updater.
   *
   * @param array $p Phase or Package
   * @param object $db
   */
  protected function setUpdater( $p, $db )
  {
    if( $p['master'] == 'project.package' )
    {
      $this->updatePackage( $p['master_id'], $db );
    }
    else
    {
      $this->updateProject( $p['master_id'], $db );
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
  protected function updatePackage( $package_id, $db )
  {
    $select = sprintf( "
        SELECT
          ((SELECT SUM(time) AS spend_minutes FROM hours WHERE phaseid IN (SELECT id FROM phase WHERE master='project.package' AND master_id=%d)) + SUM(p.spend_hours)) AS hours, 
          ((SELECT SUM(current_planning) FROM phase WHERE master='project.package' AND master_id=%d) + (SELECT SUM(current_planning) FROM package WHERE master='project.package' AND master_id=%d)) AS current_planning, 
          ((SELECT SUM(remaining_effort_time) FROM phase WHERE master='project.package' AND master_id=%d) + (SELECT SUM(remaining_effort_time) FROM package WHERE master='project.package' AND master_id=%d)) AS remaining_effort_time
        FROM
          package p
        WHERE
          (p.master='project.package' AND p.master_id=%d)", $package_id, $package_id, $package_id, $package_id, $package_id, $package_id, $package_id, $package_id );
    
    $hours = $db->getrows( $select );
    $h = $hours[0];
    
    $completed = $this->calculateCompleted( $h['hours'], $h['current_planning'], $h['remaining_effort_time'] );
    
    $update = sprintf( "UPDATE
                          package 
                        SET
                          spend_hours=%d,
                          current_planning=%d,
                          completed=%d,
                          remaining_effort_time=%d 
                        WHERE 
                          id=%d", $h['hours'], $h['current_planning'], $completed, $h['remaining_effort_time'], $package_id );
    
    $db->query( $update );
    
    $selectPackage = "SELECT id, master, master_id FROM package WHERE id=" . $package_id . " LIMIT 0,1";
    $package = $db->getrows( $selectPackage );
    
    $this->setUpdater( $package[0], $db );
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
  protected function updateProject( $project_id, $db )
  {
    $select = sprintf( "
        SELECT
          ((SELECT SUM(time) AS spend_minutes FROM hours WHERE phaseid IN (SELECT id FROM phase WHERE master='project.project' AND master_id=%d)) + SUM(p.spend_hours)) AS hours, 
          ((SELECT SUM(current_planning) FROM phase WHERE master='project.project' AND master_id=%d) + (SELECT SUM(current_planning) FROM package WHERE master='project.project' AND master_id=%d)) AS current_planning, 
          ((SELECT SUM(remaining_effort_time) FROM phase WHERE master='project.project' AND master_id=%d) + (SELECT SUM(remaining_effort_time) FROM package WHERE master='project.project' AND master_id=%d)) AS remaining_effort_time
        FROM
          package p
        WHERE
          (p.master='project.project' AND p.master_id=%d)", $project_id, $project_id, $project_id, $project_id, $project_id, $project_id, $project_id, $project_id );
    
    $hours = $db->getrows( $select );
    $h = $hours[0];
    
    $completed = $this->calculateCompleted( $h['hours'], $h['current_planning'], $h['remaining_effort_time'] );
    
    $update = sprintf( "UPDATE
                          project
                        SET
                          spend_hours=%d,
                          current_planning=%d,
                          completed=%d,
                          remaining_effort_time=%d 
                        WHERE id=%d", $h['hours'], $h['current_planning'], $completed, $h['remaining_effort_time'], $project_id );
    $db->query( $update );
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


  /**
   * Get nodename from type.
   *
   * @param string $type
   * @return string
   */
  protected function getNodeFromType( $type )
  {
    list( $module, $node ) = split( '\.', $type );
    return $node;
  }

}
