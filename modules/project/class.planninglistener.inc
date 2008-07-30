<?php
  /**
   * Planning listener
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
  class planningListener extends atkTriggerListener
  {

    /**
     * Notify the listener of an add-action on a record.
     *
     * @param String $action The action being performed
     * @param array $record The record on which the action is performed
     */
    public function postAdd(&$record)
    {
      $this->calculateHours($record);
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
      $this->calculateHours($record);
      return true;
    }

    /**
     * Calculate the spend hours for packages and projects
     *
     * @param array $record
     */
    private function calculateHours($record)
    {
    	$db = atkGetDb();
    	$selectPhase = "SELECT master, master_id FROM phase WHERE id=".$record['phaseid']." LIMIT 0,1";
    	$phase = $db->getrows($selectPhase);
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
     * Update the spend_hours in packages
     *
     * @param integer $package_id
     * @param object $db
     */
    private function updatePackage($package_id, $db)
    {
    	$qrySpendHours = sprintf("SELECT
														      (SELECT SUM(time) AS spend_minutes FROM hours WHERE phaseid IN (SELECT id FROM phase WHERE master='project.package' AND master_id=%d)) AS ph_hours,
														      SUM(p.spend_hours) AS pa_hours
														    FROM
														      package p
														    WHERE
														      (p.master='project.package' AND p.master_id=%d)", $package_id, $package_id);

    	$totalHours = $db->getrows($qrySpendHours);
    	$total = $totalHours[0]['ph_hours'] + $totalHours[0]['pa_hours'];

    	$qryUpdatePackage = sprintf("UPDATE package SET spend_hours=%d WHERE id=%d", $total, $package_id);
    	$db->query($qryUpdatePackage);

      $selectPackage = "SELECT master, master_id FROM package WHERE id=".$package_id." LIMIT 0,1";
      $package = $db->getrows($selectPackage);

      $this->setUpdater($package[0], $db);
    }

    /**
     * Update the spend_hours in projects
     *
     * @param integer $project_id
     * @param object $db
     */
    private function updateProject($project_id, $db)
    {
      $qrySpendHours = sprintf("SELECT
                                  (SELECT SUM(time) AS spend_minutes FROM hours WHERE phaseid IN (SELECT id FROM phase WHERE master='project.project' AND master_id=%d)) AS ph_hours,
                                  SUM(p.spend_hours) AS pa_hours
                                FROM
                                  package p
                                WHERE
                                  (p.master='project.project' AND p.master_id=%d)", $project_id, $project_id);

      $totalHours = $db->getrows($qrySpendHours);
      $total = $totalHours[0]['ph_hours'] + $totalHours[0]['pa_hours'];

      $qryUpdateProject = sprintf("UPDATE project SET spend_hours=%d WHERE id=%d", $total, $project_id);
      $db->query($qryUpdateProject);
    }

  }
