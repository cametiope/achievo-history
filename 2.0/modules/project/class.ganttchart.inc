<?php
  /**
   * gantchartt node class file
   *
   * @package achievo
   * @subpackage project
   */

  useattrib("atkattribute");
  useattrib("atkdateattribute");

  atkimport("module.utils.dateutil");

  include_once("achievotools.inc");

  
  /**
   * The node class for ganttchart
   *
   * @author ruud <ruud@ibuildings.nl>
   * @package achievo
   * @subpackage reports
   * @todo: - improve data storage, remove redundancy: packages will be fetches (recursively) at least twice:
   *        once for the shuttle filter field and once for the graph itself
   *        - formatShuttleArray and fromatGraphArray are candidates for refactoring
   */
  class ganttchart extends atkNode
  {

  
    /**
     * Holds the data array, formatted for the jpgraph library
     *
     * @var array
     */
    private $graph_data;
    
      
    /**
     * Holds a lookup array to determine which id from our database was placed
     * on what position in the $graph_data array
     *
     * @var array
     */
    private $graph_lookup;
    
    
    /**
     * Holds array to be used for the shuttle relation filter
     *
     * @var array
     */
    private $shuttle_data;
    
    
    /**
     * Counter to be used when formatting the jpgraph array
     * Declared as class variable since it will be used in a recursive method and is 
     * thereby called from different scopes
     *
     * @var integer
     */
    private $i;
    
    
    /**
     * Holds data about which phase has been completed for how many percent
     *
     * @var array
     */
    private $graph_completed;

   
    /**
     * Ganttchart constructor
     */
    function ganttchart($name="ganttchart")
    {
      $this->setProjectId();
      $this->atkNode($name);
      $this->setSecurityAlias("project.project");
      $this->m_securityMap["planninggraph"] = "planning";
    }

    
    /**
     * Preset values and filter the record
     *
     * @param array $project
     * @return array
     */
    private function getFilterRecord( $project = array() )
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if( $s_record == NULL )
      {
        $s_record = array();
        $s_record["from"]          = $g_sessionManager->pageVar( "from" );
        $s_record["until"]         = $g_sessionManager->pageVar( "until" );
        $s_record["display"]       = $g_sessionManager->pageVar( "display" );
        $s_record["projectid"]     = $g_sessionManager->pageVar( "projectid" );
        $s_record['packages']      = $g_sessionManager->pageVar( 'packages' );

        // Convert attribute html values to internal values
        $attrs = &$this->getFilterAttrs( );
        foreach( array_keys( $attrs ) as $attribname )
        {
          $p_attrib = &$attrs[$attribname];
          
          if( $attribname == 'packages' ) // @todo: remove dependency
          {
            $s_record[$attribname] == $this->m_postvars['packages'];
          }
          else
          {
            $s_record[$attribname] = &$p_attrib->fetchValue( $s_record );
          }
        }

        $go = $g_sessionManager->pageVar( "go" );
        if( $go != 1 )
        {
          if( count( $project ) > 0 )
          {
            $s_record['from']  = $project[0]['startdate']; 
            $s_record['until'] = $project[0]['enddate'];
          }
          $s_record["projectid"] = $this->getProjectId();
        }
      }
      return $s_record;
    }
    
    
    /**
     * Set attributes for the filterform
     *
     * @return array
     */
    private function &getFilterAttrs()
    {
      useattrib("atklistattribute");
      
      $attrs["display"] = &new atkListAttribute("display", array('project', 'package', 'all'), array( atktext("project"), atktext("package"), atktext("all") ), "", AF_OBLIGATORY|AF_LIST_NO_NULL_ITEM);
      
      atkimport("module.project.relations.nodefree_shuttlerelation");

      $attrs["packages"] = &new Nodefree_ShuttleRelation( $this->shuttle_data );
      
      $attrs["from"]  = &new atkDateAttribute("from");
      $attrs["until"] = &new atkDateAttribute("until");
      
      
      return $attrs;
    }

    
    /**
     * Return HTML that will draw the filterbar
     *
     * @param array $project
     * @return string
     */
    private function getHTMLFilterBar( $project )
    {
      $record = $this->getFilterRecord( $project );

      $packages['packages'] = $this->getPackages( $this->getProjectId(), '0000-00-00', '3000-12-31', 'project.project', 0, false );    
      $this->formatShuttleArray( $packages );      
      $attrs = &$this->getFilterAttrs();

      $output = '<form action="'.getDispatchFile().'" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="atkaction" value="planning">';
      $output.= '<input type="hidden" name="atkselector" value="project.id=\''.$this->getProjectId().'\'">';
      $output.= '<input type="hidden" name="projectid" value="'.$this->getProjectId().'">';
      $output.= '<input type="hidden" name="go" value="1">'; // Trigger to prevent loading the first time
      
      // The display attribute is currently unused
      //$output.= atktext("display").": ".$attrs["display"]->edit($record)."<br/>";
      $output.= atktext("showonlyphasesfrom", "reports")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("until")." ".$attrs["until"]->edit($record)."<br/>";
      $output.= atktext("packages")." ".$attrs["packages"]->edit($record);
      
      $output.= '<input type="submit" value="'.atktext("refresh").'" onclick="javascript:globalSubmit(this)">';
      $output.= '</form>';

      return $output;
    }
  
    
    /**
     * Return HTML that will display the projectinfo
     *
     * @param array $record
     * @return string
     */    
    private function getHTMLProjectInfo( $record )
    {      
      // Put the projectname and coordinator in a html layout
      $output = '<table border="0">';
      $output.= '<tr><td align="right"><b>'.atktext("name").': </b></td><td>'.$record[0]["abbreviation"].': '.$record[0]['name'].'</td></tr>';
      $output.= '<tr><td align="right"><b>'.atktext("coordinator").': </b></td><td>'.$record[0]['coordinator']['lastname'].', '.$record[0]['coordinator']['firstname'].'</td></tr>';
      $output.= '</table>';  
      
      return $output;  
    }
    
   
    /**
     * Return the current selected project id
     *
     * @return integer
     */
    private function getProjectId( )
    {
      $vars = atkGetPostVar();
      if ( array_key_exists('atkselector', $vars) && $vars['atkselector'] != '')
      {       
        $aSelector = decodeKeyValuePair( $vars['atkselector'] );
        return (int) array_shift( $aSelector );
      }      
      return false;
    }
    
    
    /**
     * Implement the planning action
     *
     */
    public function action_planning()
    {            
      atkimport("atk.ui.atktheme");
      // Get a singleton reference to the page, theme and ui
      $page  = &$this->getPage();
      $theme = &atkTheme::getInstance();
      $ui    = &$this->getUi();

      // Determine the selected project
      $projectid = $this->getProjectId( );

      // Select projectdetails
      $node    = atkGetNode( 'project.project' );
      $project = $node->selectDb( "project.id=".$projectid, "", "", "", array( "abbreviation", "name", "coordinator", "startdate", "enddate" ) );
           
      // Register the default style with this page
      $page->register_style($theme->stylePath("style.css"));
      
      $filter = $this->getFilterRecord( $project );
      // Compose the complete content
      $content = $this->getHTMLProjectInfo( $project ) . '<br>';
      $content.= $this->getHTMLFilterBar( $project ) .'<br>';
      
      // Add the image if the filter has been set
      if( atkArrayNvl( $this->m_postvars, "go", 0 ) == 1 )
      {
        $params = array(
          'from'    => dateUtil::arr2str( $filter['from'] ),
          'until'   => dateUtil::arr2str( $filter['until'] ),
          'display' => $filter['display'],
          'projectid' => $filter['projectid'],
        );
        $imgurl = session_url( dispatch_url( "{$this->m_module}.{$this->m_type}", "planninggraph", $params ) );   
        $content.= '<br/><br/><img src="'.$imgurl.'" border="0"/><br/><br/>';
      }

      // Put the result into a box
      $boxedcontent = $ui->renderBox(array("title"=>atktext("title_projectplanning"),"content"=>$content));
      $actionpage = $this->renderActionPage("admin", array($boxedcontent));

      // Add the boxed content to the page
      $page->addContent($actionpage);
    }

    
    /**
     * Display a date dashed
     * 
     * @param string
     * @return string
     */
    private function dashDate($undasheddate)
    {
      return substr($undasheddate, 0, 4)."-".
             substr($undasheddate, 4, 2)."-".
             substr($undasheddate, 6, 2);
    }

    
    /**
     * Wrapper function which fetches all info concerning a project
     *
     * @param integer $project_id
     * @param string $from
     * @param string $until
     * @return array
     */
    private function getProject($project_id, $from, $until )
    {
      $db = &atkGetDb();
     
      $query = '
        SELECT
          project.id,
          project.name,
          project.startdate,
          project.enddate          
        FROM
          project
        WHERE
          project.id='.$project_id;
                                
      $project = $db->getRows( $query );

      // Make a gant array, this array contains important information about the phases
      $result = array();
      if( !empty( $project) )
      {
        $result['project']  = $project[0];
        $result['packages'] = $this->getPackages( $project_id, $from, $until, 'project.project', 0, true );
        $result['tasks']    = $this->getTasks( $project_id, $from, $until, 'project.project', 0 );
      }
            
      return $result;
    }
       
    
    /**
     * Recusive method that fetches all child packages from $master
     *
     * @param integer $id
     * @param string $from
     * @param string $until
     * @param string $master
     * @param integer $level
     * @param boolean $include_tasks
     * @param array $packages
     * @return array
     */
    private function getPackages( $id, $from, $until, $master, $level, $include_tasks = true, $packages = array() )
    {
      $db = &atkGetDb();
      
      $masterClause = 
        ( $master == 'project.project' ) 
          ? '( package.master = "project.project" AND package.master_id = '.$id.')' 
          : '( package.master = "project.package" AND package.master_id = '.$id.')';

      $packagesClause =
        ( 0 !== count( $packages ) )
          ? 'package.id IN ('. implode(', ', $packages ) .')'
          : '(1 = 1)';
      
      $query = '
        SELECT 
          package.id,
          package.abbreviation_package,
          package.name,
          package.startdate,
          package.enddate,
          package.current_planning,
          package.spend_hours,
          "1" AS ganttbartype,
          "'.$level.'" AS level
        FROM 
          package
        WHERE
          '.$masterClause.'
        AND
          '.$packagesClause.'
        AND
          ( package.enddate IS NOT NULL AND package.enddate >= "'.$from.'")
        AND
          ( package.startdate IS NOT NULL AND package.startdate <= "'.$until.'")'; 
      
      $resultset = $db->getRows( $query );      
      
      $result = array();
      for( $i=0; $i < count( $resultset ); $i++ )
      {
        $result[$resultset[$i]['id']] = $resultset[$i]; 
        $result[$resultset[$i]['id']]['packages'] = $this->getPackages( $resultset[$i]['id'], $from, $until, 'project.package', ($level+1), $include_tasks, $packages );
        
        if( true === $include_tasks )
        {
          $result[$resultset[$i]['id']]['tasks']    = $this->getTasks( $resultset[$i]['id'], $from, $until, 'project.package', ($level+1) );
        }
      }  
      
      return $result;                 
    }
        
      
    /**
     * Get tasks belonging to $master
     *
     * @param integer $id
     * @param string $from
     * @param string $until
     * @param string $master
     * @param integer $level
     * @return array
     */
    private function getTasks( $id, $from, $until, $master, $level )
    {
      $db = &atkGetDb();
      
      $masterClause = 
        ($master == 'project.project') 
          ? '( phase.master = "project.project" AND phase.master_id = '.$id.')' 
          : '( phase.master = "project.package" AND phase.master_id = '.$id.')';
      
      $query = '
        SELECT 
          phase.id,
          phase.name,
          phase.completed,
          phase.spend_hours,
          phase.current_planning,
          phase.startdate,
          phase.enddate,
          "0" AS ganttbartype,
          "'.$level.'" AS level,
          phase.completed          
        FROM 
          phase
        WHERE
         '.$masterClause.' 
        AND
          ( phase.enddate IS NOT NULL AND phase.enddate <= "'.$until.'")
        AND
          ( phase.startdate IS NOT NULL AND phase.startdate >= "'.$from.'")'; 
      
      $phases = $db->getRows( $query );
      
      $result = array();
      for( $i=0; $i < count( $phases ); $i++ )
      {        
        $result[$phases[$i]['id']] = $phases[$i];
      }  
      
      return $result;        
    }

    
    /**
     * Draw the graph
     *
     * @param array $gantt
     * @param string $from
     * @param string $until
     * @param string $display
     * @param array $filter
     */
    function showGraph($gantt, $from, $until, $display, $filter )
    {      
      // Format the Graph array into a format that the jpgraph library will understand
      $this->i = -1;
      $this->formatGraphArray( $gantt, $filter );

      // Load jpgraph gantt chart library files
      include_once(moduleDir("graph")."jpgraph/jpgraph.php");
      include_once(moduleDir("graph")."jpgraph/jpgraph_gantt.php");

      $graph = new GanttGraph(0,0,"auto");
      $graph->SetBox();
      $graph->SetShadow();

      // Show day, week and month scale
      $graph->ShowHeaders(GANTT_HDAY | GANTT_HWEEK | GANTT_HMONTH);

      // Use the short name of the month together with a 2 digit year
      // on the month scale
      $graph->scale->month->SetStyle(MONTHSTYLE_SHORTNAMEYEAR2);
      $graph->scale->month->SetFontColor("white");
      $graph->scale->month->SetBackgroundColor("blue");

      // 0 % vertical label margin
      $graph->SetLabelVMarginFactor(1);

      $graph->CreateSimple( $this->graph_data, $this->getConstrains( ), $this->graph_completed );

      // Set date range to prevent autoscale errors when no activities were added to the gantt chart
      $graph->SetDateRange($from, $until);

      $graph->Stroke();
      flush();
      die();
    }

    
    /**
     * Recursive function to format the data array, so that it can be fed to 
     * the shuttle relation
     *
     * @param array $data
     * @return array
     */    
    private function formatShuttleArray( $data )
    {
      if( count( $data['packages'] ) > 0 )
      {
        foreach( $data['packages'] AS $key => $val )
        {      
          $this->shuttle_data[] = array(  
                                  'id' => $val['id'], 
                                  'name' => $val['name'],);
                                                                       
          if( count( $val['packages'] ) > 0 )
          {
            $this->formatShuttleArray( $val );  // Recursive iteration
          }
        }
      }
    }
    
    
    /**
     * Recursive function to format the data array, so that it can be fed to 
     * the jpgraph library
     *
     * @param array $data
     * @param array $filter
     * @return array
     */
    private function formatGraphArray( $data, $filter )
    {  
      // Add packages         
      if( count( $data['packages'] ) > 0 )
      {
        foreach( $data['packages'] AS $key => $val )
        {
          if( in_array( $key, $filter ) )
          {
            $this->i++;          
            $this->graph_data[] = array(  
                                    $this->i, 
                                    $val['ganttbartype'], 
                                    str_repeat(' ', ( $val['level'] * 2 ) ) . $val['name'], 
                                    $val['startdate'],
                                    $val['enddate'],
                                    '['.$val['spend_hours'] .'/'. $val['current_planning'].']');
                                    
            $this->graph_lookup[] = array( 
                                    'id' => $val['id'],
                                    'type' => 'package' 
                                  );
          }
                                         
          if( count( $val['packages'] ) > 0 || count( $val['tasks'] ) > 0 )
          {
            $this->formatGraphArray( $val, $filter );  // Recursive iteration
          }          
        }
      }
      
      // Add tasks
      if( !array_key_exists( 'level', $data )  ||                     // include project level tasks
          (count( $data['tasks'] ) > 0 && ( ( $data['level'] > 0 ) && in_array( $data['id'], $filter) ) ) )
      {
        foreach( $data['tasks'] AS $key => $val )
        {
          $this->i++;          
          $this->graph_data[] = array(  
                              $this->i, 
                              $val['ganttbartype'], 
                              str_repeat(' ', ( $val['level'] * 2 ) ) . $val['name'], 
                              $val['startdate'],
                              $val['enddate'],
                              '['.$val['spend_hours'] .'/'. $val['current_planning'].']' );
                              
          $this->graph_lookup[] = array( 
                                  'id' => $val['id'],
                                  'type' => 'phase'); 
                                  
          $this->graph_completed[] = array( $this->i, '0.'.$val['completed'] );  
        }
      } 
    }

        
    /**
     * Fetch constrains. This is a bit tricky and should perhaps be refactored.
     * Because jpgraph uses it's own indexes, we'll need to lookup our database index and compare
     * this against the index in the jpgraph array. We'll return contrains according to these results
     *
     * @return array
     */
    private function getConstrains( )
    {
      $db = &atkGetDb();
      $result = array();
      
      // Loop through all graphrows
      foreach( $this->graph_lookup AS $graphindex => $graphrow  )
      {
        // Determine if the graphrow depends on another graphrow
        $query      = 'SELECT phaseid_col AS depends_on FROM dependency WHERE phaseid_row = '.$graphrow['id'].' AND dep_type = "'.$graphrow['type'].'"';
        $dependees  = $db->getRows( $query );
        
        if( 0 < count( $dependees ) )
        {
          foreach( $dependees AS $dependee )
          {            
            foreach( $this->graph_lookup AS $id => $lookup )
            {
              if( $lookup['id'] == $dependee['depends_on'] )
              {
                $result[] = array( $graphindex, $id, CONSTRAIN_ENDSTART);
              }
            }
          }
        } 
      }
      return $result;
    }
    
    
    /**
     * Draw the graph chart
     *
     * @param object $handler
     */
    public function action_planninggraph(&$handler)
    {  
      $params['project'] = $_REQUEST['projectid'];      
      $params['from']    = $this->dashDate(dateUtil::startOfWeek($_REQUEST['from']));
      $params['until']   = $this->dashDate(dateUtil::endOfWeek($_REQUEST['until']));
      $params['display'] = $_REQUEST['display'];
      if( !in_array( $params['display'], array( 'project', 'package', 'all' ) ) )
      {
        $params['display'] = 'project';
      }
      
      $params['packages'] = 
        array_key_exists( 'packages', $this->m_postvars )
          ? $this->m_postvars['packages']
          : array(); 

      // Get the gantt details 
      $gantt = $this->getProject( $params['project'], $params['from'], $params['until'] );
            
      // Display the graph
      $this->showGraph( $gantt, $params['from'], $params['until'], $params['display'], $params['packages'] );
    }    
    
    
    /**
     * Set the project id
     * @note the code below makes no sense at all, but somehow storing $selector directly in the session won't work
     *       and this does...
     */
    private function setProjectId()
    { 
      $selector = atkGetPostVar('atkselector');
      $val = decodeKeyValuePair($selector);
      sort($val);     
      sessionStore('projectid', "project.id='\".$val[0].\"'");    
    }
  }