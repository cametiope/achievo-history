<?php
useattrib("atkNumberAttribute");
useattrib("atkTextAttribute");
userelation('atkOneToManyRelation');

class department extends atkNode
{
  function department()
  {    
    $this->atkNode("department",NF_AUTOSELECT|NF_EDITAFTERADD);
    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("manager_id", "person.person", AF_HIDE_LIST|AF_LARGE));    
    $this->add(new atkOneToManyRelation("representatives","employee.department_representative", "department_id", AF_HIDE_LIST));   
    $this->add(new atkTextAttribute("description"));
    
    $this->add(new atkOneToManyRelation("representatives","employee.department_representative", "department_id", AF_HIDE_LIST));
    
    $this->setOrder("name");
    $this->setTable("employee_department");
  }

  function descriptor_def()
  {
    return "[name]";
  }
}