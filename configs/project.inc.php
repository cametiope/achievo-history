<?php

  // This variable configures wether hour registrations should be automatically deleted when
  // a phase is deleted. If omitted, false is default.
  $config["project_cascading_delete_hours"] = false;

  // Criteria for trafficlight displayal
  $config["trafficlight_red"]     = '>= 0 AND < 30';
  $config["trafficlight_orange"]  = '>= 30 AND < 100';
  $config["trafficlight_green"]   = '= 100';

  // Determines the steps shown in the completion selectlist. Use a value like 5, 10, 20 or 25 percent
  $config["completion_percentage_steps"] = 20;
  $config["project_cascading_delete_hours"] = false;

  // Option to hide/disabled the packages.
  $config["package_enabled"] = true;

  // The limit of branches for sub_packages. By default this value is -1 (unlimited).
  $config["package_number_of_branches"] = -1;

  // For backwards compatibility issues, the subprojects will still be available in Achievo.
  // The functionality of subprojects can be turned off.
  $config["use_subprojects"] = true;

  // The completion/progress bar is divided into parts of 5, 10 20 or 25 percent
  $config["completed_parts"] = 10;

  // The default view mode for resourceplanning
  $config["resourceplanning_view"] = "week";

  // The default view mode for projectplanning
  $config["projectplanning_view"] = "week";

  $config["project_resource_planning_green"] = '$availability >= 0 AND $availability <80';
  $config["project_resource_planning_yellow"] = '$availability>= 80 AND $availability<100';
  $config["project_resource_planning_red"] = '$availability>= 100';
