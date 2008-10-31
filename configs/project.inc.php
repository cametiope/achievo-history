<?php

  // This variable configures wether hour registrations should be automatically deleted when
  // a phase is deleted. If omitted, false is default.
  $config["project_cascading_delete_hours"] = false;

  // Criteria for trafficlight displayal
  $config["trafficlight_red"]     = array('start' => 0, 'end' => 29);
  $config["trafficlight_yellow"]  = array('start' => 30, 'end' => 99);
  $config["trafficlight_green"]   = array('start' => 100, 'end' => 100);

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

  $config["project_workload_green"] = '$workload >= 0 AND $workload < 80';
  $config["project_workload_yellow"] = '$workload >= 80 AND $workload < 100';
  $config["project_workload_red"] = '$workload >= 100';
