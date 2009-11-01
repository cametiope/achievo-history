<?php

atkimport("atk.atkmetanode");

class sf_dupe extends atkMetaNode 
{
  //public $flags = NF_READONLY;
  
  function meta(&$policy)
  {
    /*@var $policy atkMetaPolicy*/
    $policy->setTypeAndParams("type","atk.attributes.atklistattribute",array(array("account","contact")));
    $policy->setTypeAndParams("direction","atk.attributes.atklistattribute",array(array("Achievo->SF","SF->Achievo")));
    
    $policy->addFlags("type","id","name","name1","fields","message","statuscode","direction","created_at",AF_SEARCHABLE);
  }
}