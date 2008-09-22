<?php
  atkimport("atk.atkmetanode.inc");
  
  class department_representative extends atkMetaNode
  {
    public $securityAlias = "employee.employee";    
    
    public static function meta(atkMetaPolicy $policy)
    {
      $policy->hasOne("employee.department",array("source"=>"department_id"));
      $policy->hasOne("person.person",array("source"=>"representative_id"));        
    }
  }