<?php

  $setup->addColumn("hoursbase","time_left","INT(10)");
  
  //rebuild hours view, or time_left field will be not available in hours view  
  $setup->dropView("hours");
  $setup->createView("hours","SELECT * FROM hoursbase WHERE virtual_time=0 OR virtual_time IS NULL", false);


?>