<?php
  /**
   * Trafficlight listener
   *
   * Calculates the planning for packages and the project when the planning
   * of a task has been changed.
   *
   * @package achievo
   * @subpackage modules
   *
   * @author danny <danny@ibuildings.nl>
   *
   * @copyright (c) 2008 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: $
   * $Id: $
   */

  /**
   * @internal Load used classes
   */
  atkimport('atk.utils.atktriggerlistener');

  /**
   * The planningListener class
   *
   * @author danny <danny@ibuildings.nl>
   * @package achievo
   * @subpackage modules
   */
  class trafficlightListener extends atkTriggerListener
  {

    /**
     * Notify the listener of an add-action on a record.
     *
     * @param String $action The action being performed
     * @param array $record The record on which the action is performed
     */
    public function postAdd(&$record)
    {
      $this->calculateTrafficLight($record);
      return true;
    }

      /**
     * Notify the listener of an update-action on a record.
     *
     * @param String $action The action being performed
     * @param array $record The record on which the action is performed
     */
    public function postUpdate(&$record)
    {
      $this->calculateTrafficLight($record);
      return true;
    }

    /**
     * Calculate the spend hours for packages and projects
     *
     * @param array $record
     */
    private function calculateTrafficLight($record)
    {
      $db = atkGetDb();
    	$phase = is_array( $record['phaseid'] ) ? $record['phaseid']['id'] : $record['phaseid'];    	
  
      $selectPhase = "SELECT id, trafficlight, trafficlight_updatestyle, current_planning, master, master_id FROM phase WHERE id=".$phase." LIMIT 0,1";
      $phase = $db->getrows($selectPhase);
      
      if( $phase[0]['trafficlight_updatestyle'] == 'A') // If updatestyle of phase is not set to (A)utomatic
      {
        $this->updatePhase($phase[0], $db);
      }
      $this->setUpdater($phase[0], $db);      
    }

    /**
     * Set node
     *
     * @param array $p Phase or Package
     * @param object $db
     */
    private function setUpdater($p, $db)
    {
      if ($p['master'] == 'project.package')
      {
        $this->updatePackage($p['master_id'], $db);
      }
      else
      {
        $this->updateProject($p['master_id'], $db);
      }
    }

    /**
     * Update the trafficlight in phases
     *
     * @param integer $package_id
     * @param object $db
     */
    private function updatePhase($phase, $db)
    {
    	$qryHours = sprintf("SELECT SUM(time) AS spend_minutes FROM hours WHERE phaseid=%d", $phase['id']);
    	$hours = $db->getValue($qryHours);

    	// We cannot calculate a percentage if no current_planning has been set
    	if( (int) $phase['current_planning'] > 0)
    	{
    	  $percentage = round((($hours/(int)$phase['current_planning']) * 100), 0);
      	$qryUpdateTrafficlight = sprintf("UPDATE phase SET trafficlight=%d WHERE id=%d", $percentage, $phase['id']);
      	$db->query($qryUpdateTrafficlight);    	  
    	}
    }

    /**
     * Update the trafficlight in packages
     *
     * @param integer $package_id
     * @param object $db
     */
    private function updatePackage($package_id, $db)
    {      
      $selectPackage = "SELECT id, trafficlight, trafficlight_updatestyle, current_planning, master, master_id FROM package WHERE id=".$package_id." LIMIT 0,1";
      $package = $db->getrows($selectPackage);
            
      if( $package[0]['trafficlight_updatestyle'] == 'A' ) // If updatestyle of package is not set to (A)utomatic
      {
        $qryUpdateTrafficlight = sprintf("UPDATE package SET trafficlight=ROUND((spend_hours/current_planning), 2) WHERE id=%d", $package_id);
        $db->query($qryUpdateTrafficlight);
      }
      $this->setUpdater($package[0], $db);
    }

    /**
     * Update the trafficlight in projects
     *
     * @param integer $project_id
     * @param object $db
     */
    private function updateProject($project_id, $db)
    {
      $selectProject = "SELECT id, trafficlight_updatestyle FROM project WHERE id=".$project_id." LIMIT 0,1";
      $project = $db->getrows($selectProject);
            
      if( $project[0]['trafficlight_updatestyle'] == 'A' ) // If updatestyle of project is not set to (A)utomatic
      {            
        $qryUpdateTrafficlight = sprintf("UPDATE project SET trafficlight=ROUND((spend_hours/current_planning), 2) WHERE id=%d", $project_id);
        $db->query($qryUpdateTrafficlight);
      }
    }

  }
