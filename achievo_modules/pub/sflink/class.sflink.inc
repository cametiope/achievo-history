<?php

require_once ('lib/soapclient/SforcePartnerClient.php');
require_once ('lib/soapclient/SforceHeaderOptions.php');
require_once ('lib/soapclient/SforceMetadataClient.php');

class sflink extends atkNode 
{
  protected $m_client;
  //protected $m_metaConnection;

  
  public function __construct($type="sflink", $flag=0)
  {
    parent::atkNode($type,$flag);
    
    //connect to salesforce
    $this->m_client = new SforcePartnerClient();
    $this->m_client->createConnection(moduleDir($this->getModule()).'lib/soapclient/partner.wsdl.xml');
    $loginResult = $this->m_client->login(atkConfig::get("sflink","login"), atkConfig::get("sflink","password"));
    //$this->m_metaConnection = new SforceMetadataClient('./achievo_modules/pub/sflink/utils/soapclient/metadata.wsdl.xml', $loginResult, $this->m_client);

    $this->setFeedback(array("accountexport","contactexport","clearsfid","clearcontactsfid"), ACTION_CANCELLED|ACTION_SUCCESS);
    $this->m_securityImplied = array("clearsfid"=>"accountexport","clearcontactsfid"=>"accountexport");
  }

  public function action_clearsfid()
  {
    if (!empty($this->m_postvars['confirm'])) $this->_doClearSfid();
    elseif (empty($this->m_postvars['cancel']))
    {
      $page = &$this->getPage();
      $page->addContent($this->confirmAction("1=0", "clearsfid", false, false, false) );
    }
    else
    {
      $location = $this->feedbackUrl("clearsfid", ACTION_CANCELLED);
      $this->redirect($location);
    }
  }

  private function _doClearSfid()
  {
    $this->getDb()->query("update organization set salesforce_id =null");
    $count = $this->getDb()->affected_rows();
    $location = $this->feedbackUrl("clearsfid", ACTION_SUCCESS,"",$count." records has been update");
    $this->redirect($location);
  }

  public function action_clearcontactsfid()
  {
    if (!empty($this->m_postvars['confirm'])) $this->_doClearContactSfid();
    elseif (empty($this->m_postvars['cancel']))
    {
      $page = &$this->getPage();
      $page->addContent($this->confirmAction("1=0", "clearcontactsfid", false, false, false) );
    }
    else
    {
      $location = $this->feedbackUrl("clearcontactsfid", ACTION_CANCELLED);
      $this->redirect($location);
    }
  }

  private function _doClearContactSfid()
  {
    $this->getDb()->query("update person set salesforce_id=null");
    $count = $this->getDb()->affected_rows();
    $location = $this->feedbackUrl("clearcontactsfid", ACTION_SUCCESS,"",$count." records has been update");
    $this->redirect($location);
  }
  
  public function action_accountexport()
  {
    if (!empty($this->m_postvars['confirm'])) $this->_doAccountExport();
    elseif (empty($this->m_postvars['cancel']))
    {
      $page = &$this->getPage();
      $page->addContent($this->confirmAction("1=0", "accountexport", false, false, false) );
    }
    else
    {
      $location = $this->feedbackUrl("accountexport", ACTION_CANCELLED);
      $this->redirect($location);
    }
  }
  
  private function _doAccountExport()
  {
    $offset = 0;
    $limit = atkConfig::get("sflink","limit");
    
    $total = 500;
    $totalcount = 0;
    
    $accountCounts = 0;
    $contactCounts = 0;
    
    do
    {
      //get data portin
      $rows = $this->getAchievoAccounts($offset,$limit);
      $offset += $limit;
      ini_set('max_execution_time',150);
            
      //iterate currect portion
      foreach ($rows as $r)
      {
        //add account
        $accountId = $this->addAccount($r);
      
        //if ok - add account's contacts
        if($accountId)
        {
          //get account's contacts
          $accountCounts++;
/*          $contacts = $this->getAccountContacts($r['id']);
          foreach ($contacts as $c)
          {
            //add contact
            $contactCounts += $this->addContact($c, $accountId);
          }*/
        }
      }
      $totalcount += count($rows);
    }
    //when we get less then $limit - this is last portion
    while ((count($rows)==$limit) && ($totalcount<$total));

    $location = $this->feedbackUrl("accountexport", ACTION_SUCCESS,"","$accountCounts accounts and $contactCounts contacts have been added");
    $this->redirect($location);
  }

  private function getAchievoAccounts($offset, $limit)
  {
    $rows = $this->getDb()->getrows("
      SELECT
       id,
       invoice_city,
       invoice_country,
       invoice_zipcode,
       invoice_state,
       CONCAT(invoice_address,IFNULL(CONCAT(', ',invoice_address2),'')) invoice_address,
       remark,
       fax,
       name,
       employees,
       phone,
       mail_city,
       mail_country,
       mail_zipcode,
       mail_state,
       CONCAT(mail_address,IFNULL(CONCAT(', ',mail_address2),'')) mail_address,
       organizationcode,
       website,
       type
      FROM
       organization o
      WHERE salesforce_id IS NULL
      LIMIT $offset, $limit
    ");
    return $rows;    
  }
  
  private function addAccount($r)
  {
    $id = false;
    
    $fields = array(
      "Name" =>$r['name'],
      "ShippingStreet" =>$r['mail_address'],
      "ShippingPostalCode" =>$r['mail_zipcode'],
      "ShippingCity" =>$r['mail_city'],
      "ShippingState" =>$r['mail_state'],
      "ShippingCountry" =>$r['mail_country'],
      "Achievo_ID__c"=>$r['id'],
      "BillingStreet" =>$r['invoice_address'],
      "BillingPostalCode" =>$r['invoice_zipcode'],
      "BillingCity" =>$r['invoice_city'],
      "BillingState" =>$r['invoice_state'],
      "BillingCountry" =>$r['invoice_country'],
      "Phone" =>$r['phone'],
      "Fax" =>$r['fax'],
      "Description" =>$r['remark'],
      "Website" =>$r['website'],
      "Type" =>$r['type'],
    );
    
    $this->atk_array_iconv($fields);
      
    $sObject = new SObject();
    $sObject->fields = $fields;
    $sObject->type = 'Account';
    $createResponse = $this->m_client->create(array($sObject));
      
    //check that account has been created ok
    if($createResponse->success)
    {
      $id = $createResponse->id;
      $this->getDb()->query("UPDATE organization SET salesforce_id='$id' WHERE id='".$r['id']."'");
      if($this->isAccountExist($r))
      {
        $this->getDb()->query("
          INSERT INTO sf_dupe (type, id, name) 
          VALUES('account','".$r['id']."','".$r['name']."')");
      } 
      
    }
    else atkerror("Error adding organization");
    return $id; 
  }

  private function isAccountExist($c)
  {
    $query = "SELECT id FROM Account 
              WHERE Name ='".escapeSQL($c['name'],true)."'"; 

  	$result = $this->m_client->query($query);
    
  	if(count($result->records)>1) return true;
  	return false;
  	
  }
  
  public function action_contactexport()
  {
    if (!empty($this->m_postvars['confirm'])) $this->_doContactExport();
    elseif (empty($this->m_postvars['cancel']))
    {
      $page = &$this->getPage();
      $page->addContent($this->confirmAction("1=0", "contactexport", false, false, false) );
    }
    else
    {
      $location = $this->feedbackUrl("contactexport", ACTION_CANCELLED);
      $this->redirect($location);
    }
  }

  private function _doContactExport()
  {
    $offset = 0;
    $limit = atkConfig::get("sflink","limit");
    
    $total = 500;
    $totalcount = 0;
    
    $contactCount = 0;
    
    do
    {
      //get data portin
      $rows = $this->getAchievoContact($offset,$limit);
      $offset += $limit;
      ini_set('max_execution_time',150);
            
      //iterate currect portion
      foreach ($rows as $r)
      {
        //add account
        $contactId = $this->addContact($r);
      
        //if ok - 
        if($contactId)
        {
          $contactCount++;
        }
      }
      $totalcount += count($rows);
    }
    //when we get less then $limit - this is last portion
    while ((count($rows)==$limit) && ($totalcount<$total));

    $location = $this->feedbackUrl("contactexport", ACTION_SUCCESS,"","$contactCount contacts have been added");
    $this->redirect($location);
  }

  private function getAchievoContact($offset, $limit)
  {
    return $this->getDb()->getrows("
      SELECT
        p.id,
        t.title,
        p.lastname,
        p.firstname,
        p.birthdate,
        p.phone,
        p.fax,
        p.email,
        p.cellular,
        p.city,
        p.country,
        p.zipcode,
        p.state,
        p.address,
        p.remark,
        o.salesforce_id
      FROM
        person p
        LEFT JOIN title t ON p.title_id=t.id
        JOIN organization o ON o.id=p.company
      WHERE
        role='contact'
        AND o.salesforce_id IS NOT NULL
        AND p.salesforce_id IS NULL
        AND p.lastname>''
      LIMIT $offset, $limit
      ");
  }
  
  
  private function addContact($c)
  {
    $count = 0;
    
    $fields = array(
    "Achievo_ID__c" => $c['id'],
    "title" => $c['title'],
    "AccountId" => $c['salesforce_id'],
    "LastName" => $c['lastname'],
    "FirstName" => $c['firstname'],
    "Phone" => $c["phone"],
    "Fax" => $c["fax"],
    "Email" => $c["email"],
    //"Birthdate" => $c["birthdate"],//only for employees in Achievo
    "MailingStreet" => $c["address"],
    "MailingCity" => $c["city"],
    "MailingState" => $c["state"],
    "MailingPostalCode" => $c["zipcode"],
    "MailingCountry" => $c["country"],
    "MobilePhone" => $c["cellular"],
    "Description" => $c["remark"],
    );

    $this->atk_array_iconv($fields);
    
    $sObject = new SObject();
    $sObject->fields = $fields;
    $sObject->type = 'Contact';
    $createResponse = $this->m_client->create(array($sObject));
    
    //check that account has been created ok
    if(!$createResponse->success)
    {
      atkerror("Contact creating Error".$c['lastname']." ".$c['firstname']);
    }
    else 
    {
      $count++;
      $id = $createResponse->id;
      $this->getDb()->query("UPDATE person SET salesforce_id='$id' WHERE id='".$c['id']."'");
      
      if($this->isContactExist($c))
      {
        $this->getDb()->query("
          INSERT INTO sf_dupe (type, id, name, name1) 
          VALUES('contact','".$c['id']."','".$c['lastname']."','".$c['firstname']."')");
      } 
    }
    return $count;
  }
  
  private function isContactExist($c)
  {
    $query = "SELECT id FROM Contact 
              WHERE Firstname ='".escapeSQL($c['firstname'],true)."' AND LastName ='".escapeSQL($c['lastname'],true)."'"; 

  	$result = $this->m_client->query($query);
    
  	if(count($result->records)>1) return true;
  	return false;
  	
  }
  
  private function atk_array_iconv(&$fields)
  {
    foreach ($fields as $key => $value)
    {
      $fields[$key] = htmlspecialchars((atk_iconv(atktext("charset"),"utf-8",$value)),ENT_QUOTES,"utf-8");
    }
  }
  

}