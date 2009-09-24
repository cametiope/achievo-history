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
      "Name" =>$r['name'],
      "ShippingStreet" =>$r['mail_address'],
      "ShippingPostalCode" =>$r['mail_zipcode'],
      "ShippingCity" =>$r['mail_city'],
      "ShippingState" =>$r['mail_state'],
      "ShippingCountry" =>$r['mail_country'],
      "ib_id__c"=>$r['id'],
      "BillingStreet" =>$r['mail_address'],
      "BillingPostalCode" =>$r['mail_zipcode'],
      "BillingCity" =>$r['mail_city'],
      "BillingState" =>$r['mail_state'],
      "BillingCountry" =>$r['mail_country'],
      "Phone" =>$r['phone'],
      "Fax" =>$r['fax'],
      "Description" =>$r['remark'],
      "Website" =>$r['website'],
      "Type" =>'vipcustomer',
    );
      
    $sObject = new SObject();
    $sObject->fields = $fields;
    $sObject->type = 'Account';
    $createResponse = $this->m_client->create(array($sObject));
      
    //check that account has been created ok
    if($createResponse->success)
    {
      $id = $createResponse->id;
      $this->getDb()->query("UPDATE organization SET sfid='$id'");
    }
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
  
  private function addContact($c,$accountId)
  {
    $count = 0;
    
    $fields = array(
    "ib_id__c" => $c['id'],
    "title" => $c['title'],
    "AccountId" => $accountId,
    "LastName" =>$c['lastname'],
    "FirstName" =>$c['firstname'],
    "Phone" => $c["phone"],
    "Fax" => $c["fax"],
    "Email" => $c["email"],
    //"Birthdate" => $c["birthdate"],
    "MailingStreet" => $c["address"],
    "MailingCity" => $c["city"],
    "MailingState" => $c["state"],
    "MailingPostalCode" => $c["zipcode"],
    "MailingCountry" => $c["country"],
    "MobilePhone" => $c["cellular"],
    "Description" => $c["remark"],
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
      $this->getDb()->query("UPDATE person SET sfid='$id'");
    }
    return $count;
  }
  
  private function getAccountContacts($accountId)
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
        p.remark
      FROM
        person p
        LEFT JOIN title t ON p.title_id=t.id
      WHERE
        role='contact'
        AND p.company='$accountId'
      ");
  }

}