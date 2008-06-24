<?php
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  
  userelation("atkonetomanyrelation");

  class dashboard extends atkNode
  {
    function dashboard($name="dashboard", $flags=0)
    {
      $this->atkNode($name,$flags|NF_READONLY );      
   
      $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
            
      $this->add(new atkAttribute("name",AF_NO_LABEL));
      $this->add(new atkDateAttribute("startdate"));
      $this->add(new atkDateAttribute("enddate"));       
      $this->addFieldSet("","[startdate.label]: [startdate.field] - [enddate.label]: [enddate.field]" );  
      $this->add(new atkDateAttribute("initial_enddate","","","","",AF_NO_LABEL));  
      
      $this->add(new atkNumberAttribute("completed",AF_NO_LABEL));                    
     
      // Time registration
      $this->add(new atkNumberAttribute("initial_planning",AF_NO_LABEL));
      $this->add(new atkNumberAttribute("current_planning",AF_NO_LABEL,6));
      
      // Todo RUUD: hours spend attribuut
      // $this->add(new spendHoursAttribute("spend_hours","id",AF_TOTAL));

      $this->add(new atkNumberAttribute("remaining_effort_time", AF_NO_LABEL));     

      //$this->add(new atkOneToManyRelation("phase","project.phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE));      
                        
      $this->setTable("project","project");
      $this->setOrder("project.abbreviation, project.name");
      $this->setIndex("name");     
    }
  }