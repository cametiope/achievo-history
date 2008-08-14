<?php
  atkimport("atk.atkmetanode.inc");
  
  class department_representative extends atkMetaNode
  {
    public $securityAlias = "person.person";    
    
    public static function meta(atkMetaPolicy $policy)
    {
      $policy->hasOne("employee.department",array("source"=>"department_id"));
      $policy->hasOne("person.person",array("source"=>"representative_id"));        
    }
  }