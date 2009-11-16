<?php

  require_once ('lib/soapclient/SforcePartnerClient.php');
  require_once ('lib/soapclient/SforceHeaderOptions.php');
  require_once ('lib/soapclient/SforceMetadataClient.php');
  
  class sflink extends atkNode 
  {
    /*var $m_client SforcePartnerClient*/
    protected $m_client;
    //protected $m_metaConnection;
    protected $m_accountaddok = 0;
    protected $m_accountadderror = 0;
    protected $m_accountupdateok = 0;
    protected $m_accountupdateerror = 0;
    protected $m_contactaddok = 0;
    protected $m_contactadderror = 0;
    protected $m_contactupdateok = 0;
    protected $m_contactupdateerror = 0;
    protected $m_destnode;
  
    
    public function __construct($type="sflink", $flag=0)
    {
      parent::atkNode($type,$flag);
      
      //connect to salesforce
      $this->m_client = new SforcePartnerClient();
      $this->m_client->createConnection(moduleDir($this->getModule()).'lib/soapclient/partner.wsdl.xml');
      $loginResult = $this->m_client->login(atkConfig::get("sflink","login"), atkConfig::get("sflink","password"));
      //$this->m_metaConnection = new SforceMetadataClient('./achievo_modules/pub/sflink/utils/soapclient/metadata.wsdl.xml', $loginResult, $this->m_client);
  
      $this->setFeedback(array("accountexport","contactexport","clearsfid","clearcontactsfid","getupdated","Accountimport","Contactimport","fix"), ACTION_CANCELLED|ACTION_SUCCESS);
      $this->m_securityImplied = array("clearsfid"=>"accountexport","clearcontactsfid"=>"accountexport","Accountimport"=>"accountexport","Contactimport"=>"contactexport","fix"=>"contactexport");
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
      $location = $this->feedbackUrl("clearsfid", ACTION_SUCCESS,"","$count records has been update");
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
    
    public function action_Accountimport()
    {
      $this->m_destnode = &atkGetNode("organization.organization");
      $this->_doAction("Account");
    }

    public function action_Contactimport()
    {
      $this->m_destnode = &atkGetNode("organization.contact");
      $this->_doAction("Contact");
    }
    
    private function _doAction($type)
    {
      if (!empty($this->m_postvars['confirm'])) $this->_doAccountContactImport($type);
      elseif (empty($this->m_postvars['cancel']))
      {
        $page = &$this->getPage();
        $page->addContent($this->confirmAction("1=0", $type."import", false, false, false) );
      }
      else
      {
        $location = $this->feedbackUrl($type."import", ACTION_CANCELLED);
        $this->redirect($location);
      }
    }
    
    private function _doAccountContactImport($type)
    {
      $limit = atkConfig::get("sflink","limit");
      
      //get data
      $getmethod = "getSalesForce".$type;
      $rows = $this->$getmethod($limit);
            
      //iterate data
      foreach ($rows->records as $r)
      {
        ini_set('max_execution_time',30);
        //add account
        $this->addAccountContactToAchievo($type, $r);
      }
      
      $location = $this->feedbackUrl($type."import", ACTION_SUCCESS,"",$this->m_accountaddok."/".$this->m_accountadderror. " ".$type."s have been added/error");
      $this->redirect($location);
    }
    
    private function addAccountContactToAchievo($type, $rez)
    {
      $premethod = "prepare".$type."Record";
      $logmethod = "logImport".$type;
      
      $r = $this->$premethod($rez);
      
      //new account - add it to Achievo
      if($this->m_destnode->addDb($r))
      {
        $message = "Add ok";
        $this->m_accountaddok++;
        $this->_updateAchievoId($type, $r['id'], $rez->Id);
      }
      else 
      {
        $message = "Add error";
        $this->m_accountadderror++;
      }
      $this->$logmethod($type, $r, $message);
    }
    
    private function _updateAchievoId($type, $id, $sfid)
    {
      $fields = array("Achievo_ID__c"=>$id);
      
      $sObject = new SObject();
      $sObject->fields = $fields;
      $sObject->type = $type;
      $sObject->Id = $sfid;
      
      $this->m_client->update(array ($sObject));      
    }
    
    private function getSalesForceAccount($limit)
    {
      $query = "SELECT id, Name, Achievo_ID__c,BillingStreet,BillingPostalCode,BillingCity,BillingState,BillingCountry,Phone,
          Fax,Description,Website,Type,ShippingStreet,ShippingPostalCode,ShippingCity,ShippingState,
        ShippingCountry FROM Account WHERE Achievo_ID__c = null Limit ".$limit; 
        //ShippingCountry FROM Account Limit ".$limit; 
      $response = $this->m_client->query($query);
  	  $queryResult = new QueryResult($response);
       
  	  return $queryResult;
  	}
    
    private function getSalesForceContact($limit)
    {
      $query = "SELECT id, Achievo_ID__c,Salutation,Title,AccountId,LastName,FirstName,Phone,Fax,Email,
          MailingStreet,MailingCity,MailingState,MailingPostalCode,MailingCountry,MobilePhone,Description
          FROM Contact WHERE Achievo_ID__c = null AND AccountId <> null Limit ".$limit;
      
      $response = $this->m_client->query($query);
  	  $queryResult = new QueryResult($response);
       
  	  return $queryResult;
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
      $limit = atkConfig::get("sflink","limit");
      $accountCounts = 0;
      
      //get data
      $rows = $this->getAchievoAccounts(0,$limit);
            
      //iterate data
      foreach ($rows as $r)
      {
        ini_set('max_execution_time',30);
        //add account
        if($this->addAccount($r))
        {
          $accountCounts++;
        }
      }
      
      $location = $this->feedbackUrl("accountexport", ACTION_SUCCESS,"","$accountCounts accounts have been added");
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
         CONCAT(IFNULL(invoice_address,''),IFNULL(CONCAT(', ',invoice_address2),'')) invoice_address,
         visit_city,
         visit_country,
         visit_zipcode,
         visit_state,
         CONCAT(IFNULL(visit_address,''),IFNULL(CONCAT(', ',visit_address2),'')) visit_address,
         remark,
         fax,
         name,
         employees,
         phone,
         mail_city,
         mail_country,
         mail_zipcode,
         mail_state,
         CONCAT(IFNULL(mail_address,''),IFNULL(CONCAT(', ',mail_address2),'')) mail_address,
         organizationcode,
         website,
         type
        FROM
         organization o
        WHERE salesforce_id IS NULL OR salesforce_id=''
        LIMIT $offset, $limit
      ");
      return $rows;    
    }
    
    private function addAccount($r)
    {
      $id = false;
      
      $fields = array(
        "Name" =>$r['name'],
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
      
      /**
       * The mapping of addresses should go like this:
       * Achievo Visit -> SF Shipping
       * Achievo Mail -> SF Shipping (only if achievo visit was completely emtpy)
       * Achievo Invoice -> SF Billing
       */
      if( ($r['visit_address']=='') AND 
          ($r['visit_zipcode']=='') AND 
          ($r['visit_city']=='') AND 
          ($r['visit_state']=='') AND 
          ($r['visit_country']==''))
      {
        $fields["ShippingStreet"] = $r['mail_address'];
        $fields["ShippingPostalCode"] = $r['mail_zipcode'];
        $fields["ShippingCity"] = $r['mail_city'];
        $fields["ShippingState"] = $r['mail_state'];
        $fields["ShippingCountry"] = $r['mail_country'];
      }
      else 
      {
        $fields["ShippingStreet"] = $r['visit_address'];
        $fields["ShippingPostalCode"] = $r['visit_zipcode'];
        $fields["ShippingCity"] = $r['visit_city'];
        $fields["ShippingState"] = $r['visit_state'];
        $fields["ShippingCountry"] = $r['visit_country'];
      }
      
      $this->strConvertAtkToSf($fields,atktext("charset"),"utf-8");
        
      $sObject = new SObject();
      $sObject->fields = $fields;
      $sObject->type = 'Account';
      $createResponse = $this->m_client->create(array($sObject));
        
      //check that account has been created ok
      if($createResponse->success)
      {
        $id = $createResponse->id;
        $this->getDb()->query("UPDATE organization SET salesforce_id='$id' WHERE id='".$r['id']."'");
        
        $msg = $this->isAccountExist($r);
        $this->getDb()->query("
            INSERT INTO sf_dupe (type, direction, id, name,message,created_at) 
            VALUES('account','Achievo->SF','".$r['id']."','".escapeSQL($r['name'],true)."','".escapeSQL($msg,true)."',NOW()");
      }
      else
      {
        $this->getDb()->query("
          INSERT INTO sf_dupe (type, direction, id, name, fields, message,statuscode,created_at) 
          VALUES('account','Achievo->SF','".$r['id']."','".escapeSQL($r['name'],true)."',
          '".escapeSQL($createResponse->errors->fields)."',
          '".escapeSQL($createResponse->errors->message)."',
          '".escapeSQL($createResponse->errors->statusCode)."',NOW())");
      }
      return $id; 
    }
  
    private function isAccountExist($c)
    {
      $query = "SELECT id FROM Account 
                WHERE Name ='".$this->escape(atk_iconv(atktext("charset"),"utf-8",$c['name']))."'"; 
    	try 
    	{
        $result = $this->m_client->query($query);
      
    	  if(count($result->records)>1) return "dupe";
    	  return "single";
    	}
    	catch (Exception $e)
    	{
    	  return $e->getMessage();
    	}
    	
    }
  
    private function _doContactExport()
    {
      $limit = atkConfig::get("sflink","limit");
      $contactCount = 0;
      
      //get data
      $rows = $this->getAchievoContact(0,$limit);
            
      foreach ($rows as $r)
      {
        ini_set('max_execution_time',30);
        //add account
        if($this->addContact($r))
        {
          $contactCount++;
        }
      }
  
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
          p.function,
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
      $result = false;
      
      /**
       * For contacts fields the mapping of achievo field 'Title' should map to SF field 'Salutation'. 
       * Whereby 'Dhr.' should become 'Mr.'.
       * The correct mapping for SF Title is: Achievo Contact field 'Job Title' -> SF contact 'Title'.
       */
      
      $fields = array(
      "Achievo_ID__c" => $c['id'],
      "Salutation" => $c['title'],
      "Title" => $c['function'],
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
  
      $this->strConvertAtkToSf($fields,atktext("charset"),"utf-8");
      
      $sObject = new SObject();
      $sObject->fields = $fields;
      $sObject->type = 'Contact';
      $createResponse = $this->m_client->create(array($sObject));
      
      if($createResponse->success)
      {
        $id = $createResponse->id;
        $result = true;
        $this->getDb()->query("UPDATE person SET salesforce_id='$id' WHERE id='".$c['id']."'");
        
        $msg = $this->isContactExist($c);
        $this->getDb()->query("
            INSERT INTO sf_dupe (type,direction, id, name, name1,message,created_at) 
            VALUES('contact','Achievo->SF','".$c['id']."','".escapeSQL($c['lastname'],true)."','".escapeSQL($c['firstname'],true)."','".escapeSQL($msg,true)."',NOW())");
      }
      else
      {
        $this->getDb()->query("
          INSERT INTO sf_dupe (type, direction, id, name, name1, fields, message,statuscode,created_at) 
          VALUES('contact','Achievo->SF','".$c['id']."','".escapeSQL($c['lastname'],true)."','".escapeSQL($c['firstname'],true)."',
          '".escapeSQL($createResponse->errors->fields)."',
          '".escapeSQL($createResponse->errors->message)."',
          '".escapeSQL($createResponse->errors->statusCode)."',NOW())");
      }
      return $result; 
    }
    
    private function isContactExist($c)
    {
      $query = "SELECT id FROM Contact 
                WHERE Firstname ='".$this->escape(atk_iconv(atktext("charset"),"utf-8",$c['firstname']))."' AND LastName ='".$this->escape(atk_iconv(atktext("charset"),"utf-8",$c['lastname']))."'"; 
  
    	try 
    	{
        $result = $this->m_client->query($query);
      
    	  if(count($result->records)>1) return "dupe";
    	  return "single";
    	}
    	catch (Exception $e)
    	{
    	  return $e->getMessage();
    	}
    }
    
    private function strConvertAtkToSf(&$fields)
    {
      foreach ($fields as $key => $value)
      {
        if(is_string($value))
        {
          $value = atk_iconv(atktext("charset"),"utf-8",$value);
        }
        $fields[$key] = htmlspecialchars($value,ENT_QUOTES,"utf-8");
      }
    }
    
    private function strConvertSfToAtk(&$fields)
    {
      foreach ($fields as $key => $value)
      {
        if(is_string($value))
        {
          $value = atk_iconv("utf-8",atktext("charset"),$value);
        }
        $fields[$key] = $value;
      }
    }
  
    private function escape($string)
    {
      $string = str_replace("\\","\\\\",$string);
      return str_replace("'","\'",$string);
    }
    
    private function getLastUpdateTime()
    {
      $db = &$this->getDb();
      $rows = $db->getrows("
        SELECT
          created_at 
        FROM
          sf_dupe 
        WHERE 
          message='Start update SF->achievo' 
        ORDER BY created_at DESC LIMIT 1
      ");
      
      if(count($rows))
      {
        $lasttime = $rows[0]['created_at'];
      }
      else 
      {
        //first time to get updated records from SF, so we have to see, when we finished export to SF
        //we think, that we don't update records before export finished
        $rows = $db->getrows("
          SELECT 
            created_at
          FROM
            sf_dupe
          WHERE
            direction='Achievo->SF'
          ORDER BY 
            created_at DESC
          LIMIT 1
        ");
  
        if(count($rows))
        {
          $lasttime = $rows[0]['created_at'];
        }
        else 
        {
          atkerror("Can't determine startdate for update");
          return '';
        }
      }
      
      return mktime(
        substr($lasttime, 11, 2),
        substr($lasttime, 14, 2),
        substr($lasttime, 17, 2),
        substr($lasttime, 5, 2),
        substr($lasttime, 8, 2),
        substr($lasttime, 0, 4));
    }
    
    private function logStartUpdate()
    {
      $this->getDb()->query("INSERT INTO sf_dupe(message,created_at) VALUES ('Start update SF->achievo',NOW())");
    }
    
    public function action_getupdated()
    {
      if (!empty($this->m_postvars['confirm'])) $this->_doGetUpdated();
      elseif (empty($this->m_postvars['cancel']))
      {
        $page = &$this->getPage();
        $page->addContent($this->confirmAction("1=0", "getupdated", false, false, false) );
      }
      else
      {
        $location = $this->feedbackUrl("getupdated", ACTION_CANCELLED);
        $this->redirect($location);
      }
    }
    
    private function _doGetUpdated()
    {
      //Set current time to get records from Salesforce
      $endTime = mktime();
      //get last update time from log
      $startTime = $this->getLastUpdateTime();
      
      $this->logStartUpdate();
      
      //first update organization
      $this->updateAccount($startTime, $endTime);    
      //next update contact
      $this->updateContact($startTime, $endTime);
      
      $location = $this->feedbackUrl("getupdated", ACTION_SUCCESS,"",
        "Accounts add/error/update/error:".$this->m_accountaddok."/".$this->m_accountadderror."/".$this->m_accountupdateok."/".$this->m_accountupdateerror.", Contacts add/error/update/error:".$this->m_contactaddok."/".$this->m_contactadderror."/".$this->m_contactupdateok."/".$this->m_contactupdateerror);
      $this->redirect($location);
    }
  
    private function updateAccount($startTime, $endTime)
    {
      $getUpdateddResponse = $this->m_client->getUpdated('Account', $startTime, $endTime);
  
      $ids = array();
      if(isset($getUpdateddResponse->ids))
      {
        if(is_array($getUpdateddResponse->ids))
        {
          $ids = $getUpdateddResponse->ids;
        }
        else 
        {
          $ids = array($getUpdateddResponse->ids);
        }
      }
  
      $account = &atkGetNode("organization.organization");
      foreach ($ids as $id)
      {
        list($rez) = $this->m_client->retrieve(
          "Name, Achievo_ID__c,BillingStreet,BillingPostalCode,BillingCity,BillingState,BillingCountry,Phone,
          Fax,Description,Website,Type,ShippingStreet,ShippingPostalCode,ShippingCity,ShippingState,
          ShippingCountry","Account",array($id));
  
        $r = $this->prepareAccountRecord($rez);
        $r['salesforce_id'] = $id;
        
        list($old) = $account->selectDb("organization.salesforce_id='".$id."'","","","",array("id"));
        if(count($old))
        {
          //organization already exists in Achievo
          $r['id'] = $old['id'];
          $r['atkprimkey'] = $old['atkprimkey'];
          if($account->updateDb($r))
          {
            $message = "Update OK";
            $this->m_accountupdateok++;
          }
          else 
          {
            $message = "Update Error";
            $this->m_accountupdateerror++;
          }
        }
        else 
        {
          //new account - add it to Achievo
          if($account->addDb($r))
          {
            $message = "Add ok";
            $this->m_accountaddok++;
          }
          else 
          {
            $message = "Add error";
            $this->m_accountadderror++;
          }
        }
        $this->logImportAccount("Account",$r, $message);
      }
    }
    
    private function prepareContactRecord($rez)
    {
        $c = array();
        $c['function'] = $rez->fields->Title;
        $c['lastname'] = $rez->fields->LastName;
        $c['firstname'] = $rez->fields->FirstName;
        $c["phone"] = $rez->fields->Phone;
        $c["fax"] = $rez->fields->Fax;
        $c["email"] = $rez->fields->Email;
        $c["address"] = $rez->fields->MailingStreet;
        $c["city"] = $rez->fields->MailingCity;
        $c["state"] = $rez->fields->MailingState;
        $c["zipcode"] = $rez->fields->MailingPostalCode; 
        $c["country"] = $rez->fields->MailingCountry;
        $c["cellular"] = $rez->fields->MobilePhone;
        $c["remark"] = $rez->fields->Description;
        $c["salesforce_id"] = $rez->Id;
        $c["role"] = 'contact';
        
        //relations
        $c['company'] = array('id'=>$this->getOrganizationId($rez->fields->AccountId));
        $c['title_id'] = array('id'=>$this->getTitleId($rez->fields->Salutation));
          
        //encode from UTF-8 to ATK CHARSET
        $this->strConvertSfToAtk($c);
        
        return $c;
    }
    
    private function prepareAccountRecord($rez)
    {
      $r = array();
      
      $r['name'] = $rez->fields->Name;
      
      $r['invoice_address'] = $rez->fields->BillingStreet;
      $r['invoice_zipcode'] = $rez->fields->BillingPostalCode;
      $r['invoice_city'] = $rez->fields->BillingCity;
      $r['invoice_state'] = $rez->fields->BillingState;
      $r['invoice_country'] = $rez->fields->BillingCountry;
      $r['phone'] = $rez->fields->Phone;
      $r['fax'] = $rez->fields->Fax;
      $r['remark'] = $rez->fields->Description;
      $r['website'] = $rez->fields->Website;
      $r['type'] = $rez->fields->Type;
      $r['visit_address'] = $rez->fields->ShippingStreet;
      $r['visit_zipcode'] = $rez->fields->ShippingPostalCode;
      $r['visit_city'] = $rez->fields->ShippingCity;
      $r['visit_state'] = $rez->fields->ShippingState;
      $r['visit_country'] = $rez->fields->ShippingCountry;
      $r['salesforce_id'] = $rez->Id;

      //encode from UTF-8 to ATK CHARSET
      $this->strConvertSfToAtk($r);
      
      return $r;
    }
    
    private function logImportContact($entity,$r,$message)
    {
      $this->getDb()->query("
        INSERT INTO 
          sf_dupe (type, direction, id, name, name1, message,created_at) 
        VALUES('$entity','SF->Achievo','".$r['id']."',
        '".escapeSQL($r['lastname'],true)."',
        '".escapeSQL($r['firstname'],true)."',
        '".escapeSQL($message)."',NOW())");
    }
    
    private function logImportAccount($entity,$r,$message)
    {
      $this->getDb()->query("
        INSERT INTO 
          sf_dupe (type, direction, id, name, message,created_at) 
        VALUES('$entity','SF->Achievo','".$r['id']."',
        '".escapeSQL($r['name'],true)."',
        '".escapeSQL($message)."',NOW())");
    }
    
    private function updateContact($startTime, $endTime)
    { 
      $getUpdateddResponse = $this->m_client->getUpdated('Contact', $startTime, $endTime);
  
      $ids = array();
      if(isset($getUpdateddResponse->ids))
      {
        if(is_array($getUpdateddResponse->ids))
        {
          $ids = $getUpdateddResponse->ids;
        }
        else 
        {
          $ids = array($getUpdateddResponse->ids);
        }
      }
      /*@var $contact atkNode*/
      $contact = &atkGetNode("organization.contact");
      foreach ($ids as $id)
      {
        list($rez) = $this->m_client->retrieve(
          "Achievo_ID__c,Salutation,Title,AccountId,LastName,FirstName,Phone,Fax,Email,
          MailingStreet,MailingCity,MailingState,MailingPostalCode,MailingCountry,MobilePhone,Description",
          "Contact",array($id));
          
        $c = $this->prepareContactRecord();
          
        list($old) = $contact->selectDb("person.salesforce_id='".$id."'","","","",array("id"));
        
        if(count($old))
        {
          //contact already exists in Achievo
          $c['id'] = $old['id'];
          $c['atkprimkey'] = $old['atkprimkey'];
          if($contact->updateDb($c))
          {
            $message = "Update OK";
            $this->m_contactupdateok++;
          }
          else 
          {
            $message = "Update Error";
            $this->m_contactupdateerror++;
          }
        }
        else 
        {
          //new contact - add it to Achievo
          if($contact->addDb($c))
          {
            $message = "Add ok";
            $this->m_contactaddok++;
          }
          else 
          {
            $message = "Add error";
            $this->m_contactadderror++;
          }
        }
        $this->logImportContact("Contact",$c, $message);
      }
    }
    
    private function getTitleId($title)
    {
      $id = "";
      if(!empty($title))
      {
        /*@var $node atkNode*/
        $node = &atkGetNode("person.title");
        
        $rec = $node->selectDb("title='$title'");
        
        if(count($rec) == 0)
        {
          //Add new title
          $new = array("title"=>$title);
          $node->addDb($new);
          $id = $new["id"];
        }
        else 
        {
          $id = $rec[0]["id"];
        }
      }
      return $id;
    }
    
    private function getOrganizationId($salesforceId)
    {
      list($row) = $this->getDb()->getrows("SELECT id FROM organization WHERE salesforce_id='$salesforceId'");
      return $row['id'];
    }

    public function action_fix()
    {
      if (!empty($this->m_postvars['confirm'])) $this->_doFix();
      elseif (empty($this->m_postvars['cancel']))
      {
        $page = &$this->getPage();
        $page->addContent($this->confirmAction("1=0", "fix", false, false, false) );
      }
      else
      {
        $location = $this->feedbackUrl("fix", ACTION_CANCELLED);
        $this->redirect($location);
      }
    }
    
    private function _doFix()
    {
      $rows = $this->getFixRecord();
      $db = $this->getDb();
      $count = 0;
      foreach ($rows->records as $r)
      {
        $accountId = $this->getOrganizationId($r->fields->AccountId);
        $db->query("
          UPDATE person
          SET salesforce_id='".$r->Id."',
          company='$accountId'
          WHERE id='".$r->fields->Achievo_ID__c."';
        ");
        $count++;
      }
      $location = $this->feedbackUrl("fix", ACTION_SUCCESS,"","$count accounts have been fixed:".$r->fields->Achievo_ID__c);
      $this->redirect($location);
    }
    
    private function getFixRecord()
    {
      $query = "SELECT id, AccountId, Achievo_ID__c FROM Contact WHERE Achievo_ID__c>4765 ORDER BY Achievo_ID__c Limit 2000"; 
      //$query = "SELECT id, AccountId, Achievo_ID__c FROM Contact ORDER BY Achievo_ID__c Limit 2000"; 
      $response = $this->m_client->query($query);
  	  $queryResult = new QueryResult($response);
       
  	  return $queryResult;
    }

    private function _doFix1()
    {
      $rows = $this->getFixRecord();
      $db = $this->getDb();
      $count = 0;
      foreach ($rows->records as $r)
      {
        $db->query("
          UPDATE organization
          SET salesforce_id='".$r->Id."'
          WHERE id='".$r->fields->Achievo_ID__c."';
        ");
        $count++;
      }
      $location = $this->feedbackUrl("fix", ACTION_SUCCESS,"","$count accounts have been fixed".$r->fields->Achievo_ID__c);
      $this->redirect($location);
    }
    
    private function getFixRecord1()
    {
      $query = "SELECT id, Achievo_ID__c FROM Account WHERE Achievo_ID__c>3000 ORDER BY Achievo_ID__c Limit 2000"; 
      //$query = "SELECT id, Achievo_ID__c FROM Account ORDER BY Achievo_ID__c Limit 2000"; 
      $response = $this->m_client->query($query);
  	  $queryResult = new QueryResult($response);
       
  	  return $queryResult;
    }
  }