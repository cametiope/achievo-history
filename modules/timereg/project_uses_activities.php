<?php

$config_atkroot = "../../";
include_once("../../atk.inc");

$db = &atkGetDb();
$rows = $db->getrows("SELECT use_activities FROM project WHERE ". stripslashes( $_GET['project'] ) ." LIMIT 1");

if( count( $rows ) == 1 )
{
  echo $rows[0]['use_activities'];
}