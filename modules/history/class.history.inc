<?php
  useattrib('atkattribute');
  useattrib('atkdateattribute');
  useattrib('atknumberattribute');
  userelation('atkmanytoonerelation');

  class history extends atkNode
  {

  	/**
  	 * Project id
  	 *
  	 * @var integer
  	 */
  	private $project;

  	/**
  	 * Constructor.
  	 *
  	 * @param String $name
  	 * @param Integer $flags
  	 */
    public function __construct($name='history', $flags=0)
    {
      $user = getUser();
      $this->setProjectId();

      $this->atkNode($name,$flags|NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);

      $this->add( new atkNumberAttribute( 'id', AF_AUTOKEY ) );

      //$this->add( new atkNumberAttribute( 'project_id', AF_HIDE|AF_FORCE_LOAD ) );

      $this->add( new atkAttribute( 'master', AF_FORCE_LOAD ) );
      $this->add( new atkNumberAttribute( 'master_id', AF_HIDE|AF_FORCE_LOAD ) );

      $this->add( new atkManyToOneRelation( 'person_id', 'person.person' ) );
      $this->add( new atkDateTimeAttribute( 'timestamp', AF_READONLY ) );
      $this->add( new atkAttribute( 'action', AF_READONLY ) );

     
      // Only show recent events
      //$this->addFilter("timestamp >= '".$user['previous_login']."'" );   
        
      $this->add( new atkNumberAttribute( 'project', AF_HIDE|AF_FORCE_LOAD) );
      
      // Only show events of the current selected project      
      if ( '' != ( $project = atkGetPostVar('atkfilter') ) )
      {
        $id = decodeKeyValuePair( $project ); 
        sort( $id ); // Sort to make sure the key has value 0
        $this->addFilter( 'project = '.$id[0] );
      }      
      
      $this->setTable('history','history');
      $this->setOrder('timestamp DESC');
    }

    /**
     * Display the master - master_id column a bit more userfriendly
     *
     * @param array $record
     * @param string $mode
     */
    public function master_display($record, $mode)
    {
      $node   = getNode( $record['master'] );
      $node->addFilter( $node->getTable().'.id', $record['master_id'] );
      $result = $node->selectDb();

      return ucfirst( substr($record['master'], ( strpos( $record['master'], '.' ) +1 ) ) ).': '. $result[0]['name'];
    }

      /**
     * Edit action handler override.
     *
     * Add an extra param to the template
     *
     * @param atkEditHandler $handler
     * @return string HTML
     */
    public function action_edit(&$handler)
    {
      $handler->addRenderBoxVar('projectmenu', $this->getProjectMenu());
      return $handler->action_edit();
    }

    /**
     * AdminHeader override for projectmenu.
     */
    function adminHeader()
    {
      $this->getPage()->register_style(atkTheme::getInstance()->stylePath('tabs.css'));
      return $this->getProjectMenu().'<br/>';
    }

    /**
     * AddHeader override for projectmenu.
     */
    function addHeader()
    {
      $this->getPage()->register_style(atkTheme::getInstance()->stylePath('tabs.css'));
      return $this->getProjectMenu().'<br/>';
    }

    /**
     * AdminHeader override for projectmenu.
     */
    function viewHeader()
    {
      return $this->getProjectMenu().'<br/>';
    }

    /**
     * Get the project menu.
     *
     * @return string HTML
     */
    private function getProjectMenu()
    {
      atkimport('module.project.projectmenu');
      $pm = new projectmenu($this->atkNodeType(), $this->m_project);
      return $pm->getTabHtml();
    }

    /**
     * Set the project id.
     */
    private function setProjectId()
    {
      if (array_key_exists('atkselector', $this->m_postvars) && $this->m_postvars['atkselector'] != '')
      {
        $this->project = $this->m_postvars['atkselector'];
        sessionStore('projectid', $this->project);
      }
      else
      {
        $this->project = sessionLoad('projectid');
      }
    }

  }