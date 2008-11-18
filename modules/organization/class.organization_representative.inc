<?php
  atkimport("atk.atkmetanode");
  
  class organization_representative extends atkMetaNode
  {
    public $securityAlias = "organization.organization";    
    
    public static function meta(atkMetaPolicy $policy)
    {
      $policy->hasOne("organization.organization",array("source"=>"organization_id"));
      $policy->hasOne("person.person",array("source"=>"representative_id"));  
    }
  }