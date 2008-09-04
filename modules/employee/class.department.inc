<?php
useattrib("atkNumberAttribute");
useattrib("atkTextAttribute");
userelation('atkOneToManyRelation');

class department extends atkTreeNode
{
  function department()
  {    
    $this->atkTreeNode("department", NF_TREE_NO_ROOT_COPY|NF_TREE_NO_ROOT_DELETE|NF_EDITAFTERADD|NF_EXPORT|NF_TREE_AUTO_EXPAND);    
    
    $this->add(new atkNumberAttribute("id",AF_AUTOKEY));
    $this->add(new atkManyToOneTreeRelation("parent_id","employee.department", AF_PARENT|AF_FORCE_LOAD));    
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