<?php

  // This variable configures wether hour registrations should be automatically deleted when
  // a phase is deleted. If omitted, false is default.
  $config["project_cascading_delete_hours"] = false;

  //Option to hide/disabled the packages.
  $config["package_enabled"] = true;

  //The limit of branches for sub_packages. By default this value is -1(unlimited).
  $config["package_number_of_branches"] = -1; 


  //For backwards compatibility issues, the subprojects will still be available in Achievo. 
  //The functionality of subprojects can be turned off.
  $config["use_subprojects"] = true;   

?>