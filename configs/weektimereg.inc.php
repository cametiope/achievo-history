<?php

  // Use this setting to descide what projects to display. Can
  // be either 'recent' to display the recent projects (default)
  // or 'byteam' to display the projects by team (and project
  // startdate and enddate)
  // or 'projectlist' - to display projects only from users list
  // (in this case we must set autoshowactiveprojects to false too.
  $config["project_selection"] = "projectlist";

  // When the setting above is 'by team', set the id of a default
  // activity to display. Other activities can be added with an
  // automatically added set of selectboxes.
  // Set to 0 or remove config to fallback to default functionality.
  // $config["project_selection_byteam_activityfilter"]  = 1;

  // Set to 'true' to hide the 'add project/phase/activity' link
  $config["hide_add_project_link"] = false;

  // If set to true, active projects do not need to have "Always
  // visible in time registration" to be set.
  $config["autoshowactiveprojects"] = false;

  // If set to true, customer column will be show on weektimereg
  // and select_projectselect pages
  $config["showOrganization"] = true;
?>