<?php
  // Add modifier
  $modifiers[] = 'project.phase';

  class mod_timereg_advanced extends achievoModule
  {

    public function getNodes()
    {
      registerNode("timereg_advanced.timereg_advanced", array("add", "report"));
      registerNode("timereg_advanced.timereg_phase", array("add"));
    }


    public function getMenuItems()
    {
      //$this->menuitem("time_registration_advanced", dispatch_url("timereg_advanced.timereg_advanced", "admin"), 'time_registration');
      $this->menuitem("time_registration_advanced", dispatch_url("timereg_advanced.timereg_advanced", "add"), 'time_registration');
    }


    /**
     * Include modifier to the project.phase node
     * This enables us to change the node, without actually having to change the node itself
     *
     * @param $node node instance
     */
    public function project_phase_modifier( &$node )
    {
      useattrib( 'atkdummyattribute' );
      $node->add( new atkDummyAttribute( 'timereg_phase', $this->getPhaseTimeRegistration($node), AF_HIDE_LIST|AF_NO_LABEL ) );
      $node->setColumnAttribs(array('timereg_phase'));
    }


    /**
     * Fetch html which displays a link to the add-dialog for timeregistration
     *
     * @todo create a custom dialog link that actually works. Set the lower two lines in project_phase_modifier
     *       to use this method
     * @return string
     */
    private function getPhaseTimeRegistration( &$node )
    {
      atkimport('atk.ui.atkdialog');
      $dialog = new atkDialog('timereg_advanced.timereg_phase', 'add', 'dialog');
      $dialog->setModifierObject('timereg_advanced.timereg_phase');
      $onClick = $dialog->getCall();

      return '
        <a href="javascript:void(0)" onclick="'.$onClick.'; return false;" class="valignMiddle">'.atktext('add_timeregistration_dialog').'</a>
      ';
    }
  }
