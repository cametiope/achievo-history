<?php
  // Add modifier
  $modifiers[] = 'project.phase';

  class mod_timereg_advanced extends achievoModule
  {

    public function getNodes()
    {
      registerNode("timereg_advanced.timereg_advanced", array("add", "favorites", "times"));
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
      useattrib( 'atkboolattribute' );
      $node->add( new atkDummyAttribute( 'timereg_phase', $this->getPhaseTimeRegistration($node), AF_HIDE_LIST|AF_NO_LABEL ) );
      $node->add( new atkBoolAttribute( 'is_favorite',AF_HIDE_LIST));
      $node->setColumnAttribs(array('timereg_phase'));
    }


    /**
     * Fetch html which displays a link to the add-dialog for timeregistration
     *
     * @return string
     */
    private function getPhaseTimeRegistration( &$node )
    {
      global $ATK_VARS;

      atkimport('atk.ui.atkdialog');
      $dialog = new atkDialog('timereg_advanced.timereg_phase', 'add', 'dialog', array('atkfilter' => 'phaseid="'.array_shift( decodeKeyValuePair( $ATK_VARS['atkselector'] ) ).'"'));
      $dialog->setModifierObject('timereg_advanced.timereg_phase');
      $onClick = $dialog->getCall();

      return '
        <a href="javascript:void(0)" onclick="'.$onClick.'; return false;" class="valignMiddle">'.atktext('add_timeregistration_dialog').'</a>
      ';
    }
  }
