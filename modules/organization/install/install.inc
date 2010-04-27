<?php

  $setup->installNode("organization.contact");
  $setup->installNode("organization.contracttype");
  $setup->installNode("organization.contracts");
  $setup->installNode("organization.organization");
  
  $db = &atkGetDb();
  
  $res = $db->getrows("SHOW CREATE TABLE organization");
  $query = str_replace("organization","bin_organization",$res[0]['Create Table'],1);

  $res = $db->getrows("SHOW CREATE TABLE contact");
  $query = str_replace("contact","bin_contact",$res[0]['Create Table'],1);
  

  $setup->setVersion(24);

?>