<?php
  $setup->addColumn("phase","master",               "varchar(100)"); 
  $setup->addColumn("phase","master_id",            "int(11)");
  $setup->addColumn("phase","remaining_effort_time","int(11)"); 
  $setup->addColumn("phase","completed",            "smallint(3)"); 
  $setup->addColumn("phase","priority",             "smallint(3)", true); 
  $setup->addColumn("phase","assign_to",            "int(11)", true);
  // Data conversion
  $setup->executeSQL('UPDATE phase SET master = "project.project", master_id = projectid');  
  
  $setup->addColumn("project","completed",            "smallint(3)");    
  $setup->addColumn("project","initial_planning",     "int(11)");  
  $setup->addColumn("project","remaining_effort_time","int(11)");
  $setup->addColumn("project","current_planning",     "int(11)");  
  $setup->addColumn("project","traffic_light",        "smallint(3)");  
  $setup->addColumn("project","project_memo",         "text");  
  $setup->addColumn("project","project_group_id",     "int(11)");
  $setup->addColumn("project","initial_enddate",      "date");  
  $setup->addColumn("project","spend_hours",          "varchar(100)");
  $setup->installNode("project.project_group");
  
  // Alter dependency table
  $setup->addColumn("dependency", "dep_type", "varchar(100)");
  $setup->executeSQL('ALTER TABLE `dependency` DROP PRIMARY KEY');
  $setup->executeSQL('ALTER TABLE `dependency` ADD PRIMARY KEY ( `phaseid_row` , `phaseid_col` , `dep_type`)');
  // Data conversion
  $setup->executeSQL('UPDATE dependency SET dep_type = "phase"');
  
  
  $setup->addColumn("package","projectid","int(11)");
?>