<?php

  class pim extends atkNode
  {
    function pim()
    {
      $this->atkNode("pim",NF_NO_SECURITY);
    }

    function action_pim()
    {
      global $g_user, $g_db, $g_layout;

      $me = $g_user["name"];

      $g_layout->initGUI();
      $g_layout->ui_top(text("title_pim"));
      $res = '<br>';

      $pimItems = atkHarvestModules("getPimItems");

      foreach ($pimItems as $name => $contents)
      {
        $res.= '<b>'.text('pim_'.$name).'</b><br><br>';
        $res.= $contents;
        $res.= '<br><br>';
      }

      $res.='<br>';

      $g_layout->output($res);

      $g_layout->ui_bottom();

      $g_layout->page("PIM");
    }
    
    function action_adminpim()
    {
      global $g_layout;      

      $g_layout->initGUI();
      $g_layout->ui_top(text("title_pim_adminpim"));
      
      $content.='<br><div align="left" style="margin-left: 10px;"><b>Welcome to Achievo!</b>
                 <br><br>You are currently logged in as administrator. <b>This account 
                 is for setup-purposes only</b>.
                 <br><br>As this is not a real account, do not use features 
                 like timeregistration etc.
                 <br>It is best to use this account only when you run Achievo 
                 for the first time, or when you need to upgrade.
                 
                 <br><br><b>First time usage</b>
                 <br>If this is the first time you login to Achievo, '.
                 href(dispatch_url("employee.employee", "admin"), "click here", SESSION_NESTED).
                 ' to create the first real user account. 
                 <br>Then, relogin as the new user.
                 <br>
                 <br><b>Upgrades and module installations</b>
                 <br>If you need to upgrade or if you installed a new module, 
                 use the \'setup\' link in the top frame to update the database.
                 <br><br><b>Donations</b>
                 <br>If you like Achievo, consider becoming a sponsor for the
                 project. Contributions keep the Achievo project alive. 
                 <br>Click the button below for more information.
                 <br><br><a href="http://www.achievo.org/support/donate" target="_new"><img border="0" src="modules/pim/paypal_donate.gif" alt="donate"></a></div><br><br>';      
      
      $g_layout->output($content);   
      
      $g_layout->ui_bottom();

      $g_layout->page("PIM");
    }
  }

?>
