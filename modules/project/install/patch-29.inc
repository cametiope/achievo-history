<?php

	$db = &atkGetDb();
	$res = $db->getrows("SELECT id FROM role WHERE name='coordinator'");
	if(count($res)==0)
	{
		$node = &atkGetNode("project.role");
		$rec = array("name"=>"coordinator","description"=>"Standard coordinator role");
		$node->addDb($rec);
	}