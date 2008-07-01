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
            
      $this->add(new atkAttribute("name",AF_READONLY));
      $this->add(new atkDateAttribute("startdate"));
      $this->add(new atkDateAttribute("enddate"));       
      $this->addFieldSet("","[startdate.label]: [startdate.field] - [enddate.label]: [enddate.field]" );  
      $this->add(new atkDateAttribute("initial_enddate","","","","",AF_NO_LABEL));  
      
      $this->add(new atkNumberAttribute("completed",AF_READONLY));                    
     
      // Time registration
      $this->add(new atkDurationAttribute('initial_planning', '15m', '100h', AF_READONLY));
      $this->add(new atkDurationAttribute('current_planning', '15m', '100h', AF_READONLY));
      
      // Todo RUUD: hours spend attribuut
      // $this->add(new spendHoursAttribute("spend_hours","id",AF_TOTAL));
      $this->add(new atkDurationAttribute('spend_hours', '15m', '100h', AF_READONLY));

      $this->add(new atkFieldSet('actual_planning', '[current_planning.field] &nbsp; ' . $this->text('booked') . ': [spend_hours.field]'));
      
      $this->add(new atkDurationAttribute('remaining_effort_time', '15m', '100h', AF_READONLY));

      //$this->add(new atkOneToManyRelation("phase","project.phase","projectid",AF_HIDE_LIST|AF_CASCADE_DELETE));      
                        
      $this->setTable("project","project");
      $this->setOrder("project.abbreviation, project.name");
      $this->setIndex("name");     
      $this->setDescriptorTemplate('[name]');
    }
    
    /**
     * Display override for completed field
     *
     * @param array $record
     * @param string $mode
     * @return string
     */    
    public function completed_display($record,$mode)
    {
    	return $record['completed'].'%';
    }
  }