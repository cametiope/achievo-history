<?php

  class test_resourceplanning extends atkTestCase
  {
    function test_getWorkingDaysFromTask()
    {
      $node = &atkGetNode("project.resourceplanning");

      $this->assertEqual($node->getWorkingDaysFromTask("2008-10-06", "2008-10-10", "2008-09-29", "2008-10-17"), 5, 'Task includes into period');
      $this->assertEqual($node->getWorkingDaysFromTask("2008-09-22", "2008-09-28", "2008-09-29", "2008-10-17"), 0, 'Task before into period');
      $this->assertEqual($node->getWorkingDaysFromTask("2008-09-29", "2008-10-17", "2008-09-22", "2008-09-28"), 0, 'Task after into period');
      $this->assertEqual($node->getWorkingDaysFromTask("2008-09-22", "2008-09-29", "2008-09-29", "2008-10-17"), 1, 'One-day cross between task and period');
      $this->assertEqual($node->getWorkingDaysFromTask("2008-10-17", "2008-10-24", "2008-09-29", "2008-10-17"), 1, 'One-day cross between task and period');
      $this->assertEqual($node->getWorkingDaysFromTask("2008-09-22", "2008-10-03", "2008-09-29", "2008-10-17"), 5, 'Task starts before period');
      $this->assertEqual($node->getWorkingDaysFromTask("2008-10-13", "2008-10-25", "2008-09-29", "2008-10-17"), 5, 'Task ends after period');
    }
  }

?>