<?php

  class mod_organization extends achievoModule
  {
    function getNodes()
    {
      registerNode("organization.organization", array("admin", "add", "edit", "delete", "document"));
      registerNode("organization.contact", array("admin", "add","edit","delete"));
      registerNode("organization.contracts", array("admin","add","edit","delete"));
      registerNode("organization.contracttype", array("admin","add","edit","delete"));
    }

    function getMenuItems()
    {
      $this->menuitem("organizationmanagement",dispatch_url("organization.organization", "admin"),"main",array("organization.organization","admin"),500);
      $this->menuitem("organization_admin", dispatch_url("organization.organization", "admin"),"organizationmanagement", array("organization.organization","admin"));
      $this->menuitem("contracts", dispatch_url("organization.contracts","admin"),"organizationmanagement", array("organization.contracts","admin"));

      $this->menuitem("contracttype", dispatch_url("organization.contracttype","admin"),"setup", array("organization.contracttype","admin"));
      $this->menuitem("-","","main",true,720);
      $this->menuitem("-","","organizationmanagement",true);
      $this->menuitem("contact_admin", dispatch_url("organization.contact", "admin"),"organizationmanagement", array("organization.contact","admin"));
    }

    function getDependencies($mode)
    {
      return array("person");
    }

    function search($expression)
    {
      $mode = $this->getViewMode("status");
      
      $res = array();
      $custnode = &atkGetNode("organization.organization");
      $recs = $custnode->searchDb($expression);

      $res["organizations"] = $this->recLinks($recs, "organization.organization");

      $contactnode = &atkGetNode("organization.contact");
      $res["contacts"] = $this->recLinks($contactnode->searchDb($expression), "organization.contact");

      $contractnode = &atkGetNode("organization.contracts");
      if($mode == "active")
      {
        $contractnode->addFilter("contract.status='active'");
      }
      $res["contracts"] = $this->recLinks($contractnode->searchDb($expression), "organization.contracts");

      return $res;
    }
    
    public function checkMerge($params)
    {
      $ret = SF_NOCONFLICT;
      list($sflink,$node,$id,$masterId,$master,$slave) = $params; 
      
      if($node->getType() == 'organization')
      {
        $db = &atkGetDb();
        list($mcount) = $db->getrows("SELECT count(id) qty FROM contract WHERE customer='$masterId'");  
        list($scount) = $db->getrows("SELECT count(id) qty FROM contract WHERE customer='$id'");  
        
        if($scount['qty'] && $mcount['qty'])
        {
          $p_attr = &$node->getAttribute("projects");
          $sflink->logConflict($node->getType(),$master,$slave,$p_attr);
          $ret = SF_CONFLICT;
        }
      }
      return $ret;
    }
    
    public function doMerge($params)
    {
      list($sflink,$node,$id,$masterId,$master,$slave) = $params; 
      if($node->getType() == 'organization')
      {
        $this->mergeOrganization($id,$masterId,$slave,$master);
      }
    }    
    
    private function mergeOrganization($id, $masterid, &$slave, &$master)
    {
      atkGetDb()->query("UPDATE contract SET customer='$masterid' WHERE customer='$id'");
    }
    
  }
?>
