<?php

  $setup->installNode("organization.contact");
  $setup->installNode("organization.contracttype");
  $setup->installNode("organization.contracts");
  $setup->installNode("organization.organization");

  $db = &atkGetDb();

  $res = $db->getrows("SHOW CREATE TABLE organization");
  $query = str_replace("`organization`","`bin_organization`",$res[0]['Create Table']);
  $db->query($query);

  $res = $db->getrows("SHOW CREATE TABLE person");
  $query = str_replace("`person`","`bin_person`",$res[0]['Create Table']);
  $db->query($query);


  $setup->setVersion(24);

?>