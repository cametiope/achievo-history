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

    $this->setFeedback(array("accountexport","contactexport","clearsfid"), ACTION_CANCELLED|ACTION_SUCCESS);
    $this->m_securityImplied = array("clearsfid"=>"accountexport");
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
      WHERE sfid IS NULL
      LIMIT $offset, $limit
    ");
    atkdebug("COUNT ROWS:".count($rows));
    return $rows;    
  }
  
  private function addAccount($r)
  {
    $id = false;
    
    $fields = array(
      "Name" =>htmlspecialchars($r['name']),
      "ShippingStreet" =>htmlspecialchars($r['mail_address']),
      "ShippingPostalCode" =>htmlspecialchars($r['mail_zipcode']),
      "ShippingCity" =>htmlspecialchars($r['mail_city']),
      "ShippingState" =>htmlspecialchars($r['mail_state']),
      "ShippingCountry" =>htmlspecialchars($r['mail_country']),
      "ib_id__c"=>$r['id'],
      "BillingStreet" =>htmlspecialchars($r['mail_address']),
      "BillingPostalCode" =>htmlspecialchars($r['mail_zipcode']),
      "BillingCity" =>htmlspecialchars($r['mail_city']),
      "BillingState" =>htmlspecialchars($r['mail_state']),
      "BillingCountry" =>htmlspecialchars($r['mail_country']),
      "Phone" =>htmlspecialchars($r['phone']),
      "Fax" =>htmlspecialchars($r['fax']),
      "Description" =>htmlspecialchars($r['remark']),
      "Website" =>htmlspecialchars($r['website']),
      "Type" =>htmlspecialchars($r['type']),
    );
      
    $sObject = new SObject();
    $sObject->fields = $fields;
    $sObject->type = 'Account';
    $createResponse = $this->m_client->create(array($sObject));
      
    //check that account has been created ok
    if($createResponse->success)
    {
      $id = $createResponse->id;
      $this->getDb()->query("UPDATE organization SET sfid='$id' WHERE id='".$r['id']."'");
    }
    else halt("Error adding organization");
    return $id; 
  }
  
  public function action_clearsfid()
  {
    if (!empty($this->m_postvars['confirm'])) $this->_doClearSid();
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

  private function _doClearSid()
  {
    $this->getDb()->query("update organization set sfid =null");
    $count = $this->getDb()->affected_rows();
    $location = $this->feedbackUrl("clearsfid", ACTION_SUCCESS,"",$count." records has been update");
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
        p.sfid
      FROM
        person p
        LEFT JOIN title t ON p.title_id=t.id
        JOIN organization o ON o.id=p.company
      WHERE
        role='contact'
        AND o.sfid IS NOT NULL
        AND p.sfid IS NULL
      LIMIT $offset, $limit
      ");
  }
  
  
  private function addContact($c)
  {
    $count = 0;
    
    $fields = array(
    "ib_id__c" => $c['id'],
    "title" => htmlspecialchars($c['title']),
    "AccountId" => htmlspecialchars($c['sfid']),
    "LastName" =>htmlspecialchars($c['lastname']),
    "FirstName" =>htmlspecialchars($c['firstname']),
    "Phone" => htmlspecialchars($c["phone"]),
    "Fax" => htmlspecialchars($c["fax"]),
    "Email" => htmlspecialchars($c["email"]),
    //"Birthdate" => $c["birthdate"],//only for employees in Achievo
    "MailingStreet" => htmlspecialchars($c["address"]),
    "MailingCity" => htmlspecialchars($c["city"]),
    "MailingState" => htmlspecialchars($c["state"]),
    "MailingPostalCode" => htmlspecialchars($c["zipcode"]),
    "MailingCountry" => htmlspecialchars($c["country"]),
    "MobilePhone" => htmlspecialchars($c["cellular"]),
    "Description" => htmlspecialchars($c["remark"]),
    );

    $sObject = new SObject();
    $sObject->fields = $fields;
    $sObject->type = 'Contact';
    $createResponse = $this->m_client->create(array($sObject));
    
    //check that account has been created ok
    if(!$createResponse->success)
    {
      atkerror("Contact creating Error");
    }
    else 
    {
      $count++;
      $id = $createResponse->id;
      $this->getDb()->query("UPDATE person SET sfid='$id' WHERE id='".$c['id']."'");
    }
    return $count;
  }
  

}