<?php
  atkimport("atk.atkmetanode");
  
  class department_representative extends atkMetaNode
  {
    public $securityAlias = "employee.employee";    
    
    public static function meta(atkMetaPolicy $policy)
    {
      $policy->hasOne("employee.department",array("source"=>"department_id"));
      $policy->hasOne("employee.employee",array("source"=>"representative_id"));        
    }
  }