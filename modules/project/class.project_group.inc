<?php

class project_group extends atkNode
{
  function project_group()
  {
    $this->atkNode("project_group");

    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE, 50));

    $this->setTable("project_group");
    $this->setOrder("name");
    $this->setDescriptorTemplate("[name]");    
  }

}
?>