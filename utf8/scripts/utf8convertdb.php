<?php
set_time_limit(300);
$extended = true;
$skiptables_simple = array('db_sequence');
$debug = true;

$config_atkroot = "../";
include_once ($config_atkroot . "atk.inc");

$dbconfig = atkconfig("db");

$charset = $dbconfig['default']['charset'];
$collate = $dbconfig['default']['collate'];

if($charset=='' || $collate=='')
{
  echo "Please update your database settings in config.inc.php. You are missing the charset or collate settings";
}

echo "Achievo Database convert script v1.0\n";
echo "------------------------------------\n";
echo "\n";
echo "Convert the current database (".$dbconfig['default']['db'].") to charset $charset with collate $collate\n\n";
echo "Debug mode: ".($debug?'on':'off')."\n\n";


$db = atkGetDb();

$fields = $db->getRows("SHOW TABLES");
foreach ($fields as $field)
{
  $table = array_values($field);
  $table = $table[0];

  if (!in_array($table,$skiptables_simple))
  {
    echo "-- Table: $table --\n";

    if (!$extended)
    {
      $q = "ALTER TABLE `".$table."` CONVERT TO CHARACTER SET utf8 COLLATE utf8_unicode_ci";
      if(!$debug) $db->query($q);
      echo "Converted table $table to UTF8 ($q)\n\n";
    }
    else
    {
      $columns = $db->getRows("SHOW FULL FIELDS FROM `$table`");
      foreach ($columns as $column)
      {
        $skipped_field_types = array('char', 'text', 'enum', 'set');

        foreach ($skipped_field_types as $type)
        {
          if (strpos($column['Type'], $type) !== false)
          {
            if (empty($column['Key']) && empty($column['Extra']))
            {
              if ($column['Null']=='YES') {
                  $nullable = ' NULL ';
              } else {
                  $nullable = ' NOT NULL';
              }

              if ($column['Null']=='YES' && $column['Default']==NULL) {
                  $default = " DEFAULT NULL";
              } else if ($column['Default']!='') {
                  $default = " DEFAULT '".mysql_real_escape_string($column['Default'])."'";
              } else {
                  $default = '';
              }

              // Only do this if your data is not encoded according to the set charset:
              $q2 = 'ALTER TABLE `'.$table.'` CHANGE `'.$column['Field'].'` `'.$column['Field'].'` BLOB';
              if(!$debug) $db->query($q2);
              echo "Converted column " . $column['Field'] . " to BLOB ($q2)\n";

              $q3 = 'ALTER TABLE `'.$table.'` CHANGE `'.$column['Field'].'` `'.$column['Field'].'` '.$column['Type'].' CHARACTER SET utf8 COLLATE utf8_unicode_ci '.$nullable.' '.$default;
              if(!$debug) $db->query($q3);
              echo "Converted column " . $column['Field'] . " to UTF8 ($q3)\n";
            }
            else
            {
              echo "---> Skipping column ".$column['Field']." because it is a key or has a 'Extra' value.\n";
            }
          }
        }
      }

      $q1 = "ALTER TABLE `".$table."` DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci";
      if(!$debug) $db->query($q1);
      echo 'Converted default charset for table ' . $table . ' to UTF8 (' . $q1 . ')'."\n\n";
    }
  }
}

$config = atkconfig("db");
$dbname = $config['default']["db"];
$q0 = 'ALTER DATABASE `'.$dbname.'` DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci';
if(!$debug) $db->query($q0);
echo "\n\nConverted default charset for database $dbname to UTF8 ($q0)\n";
echo "Convert ended\n\n";
