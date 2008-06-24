<?php

  useattrib("atkattribute");
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  userelation("atkmanytoonerelation");
  
  class history extends atkNode
  {
    public function history($name="history", $flags=0)
    {
      $user = getUser();
      
      $this->atkNode($name,$flags|NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);
      
      $this->add( new atkNumberAttribute( "id", AF_AUTOKEY ) );
      $this->add( new atkAttribute( "master", AF_READONLY ) );
      $this->add( new atkNumberAttribute( "master_id", AF_READONLY ) ); 
      $this->add( new atkManyToOneRelation( "person_id", "person.person"));
      $this->add( new atkDateTimeAttribute( "timestamp", AF_READONLY ) );
      $this->add( new atkAttribute( "action", AF_READONLY ) );
          
      // Only show recent events
      $this->addFilter("timestamp >= '".$user['previous_login']."'" );
      
      $this->setTable("history","history");
      $this->setOrder("timestamp DESC");
    }
    
  }