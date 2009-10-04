<?php

atkimport("atk.atkmetanode");

class sf_dupe extends atkMetaNode 
{
  public $flags = NF_READONLY;
  
  function meta(&$policy)
  {
    /*@var $policy atkMetaPolicy*/
    $policy->setTypeAndParams("type","atk.attributes.atklistattribute",array(array("account","contact")));
    $policy->addFlags("type","id","name","name1",AF_SEARCHABLE);
  }
}