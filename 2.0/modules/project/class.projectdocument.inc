<?php

useattrib("atknumberattribute");
useattrib("docmanager.projectdocumentmanager");

/**
 * The node class for deliverable.
 */
class projectdocument extends atkNode
{
  
  /**
   * Project identifier
   *
   * @var string
   */
  protected $project;


  /**
   * Constructor.
   *
   */
  public function __construct()
  {
    $this->atkNode( 'projectdocument' );
       
    $this->add( new atkNumberAttribute( 'id', AF_AUTOKEY ) );
    
    $this->add( new projectDocumentManager( 'docs', AF_HIDE_LIST | AF_HIDE_SEARCH | AF_NO_LABEL , 'project.project' ));
    
    $this->setTable('project','project');
    $this->setSecurityAlias('project.project');
  }

  /**
   * Edit action handler override.
   *
   * Add an extra param to the template
   *
   * @param atkEditHandler $handler
   * @return string HTML
   */
  public function action_edit( &$handler )
  {
    $handler->addRenderBoxVar( 'projectmenu', $this->getProjectMenu() );
    return $handler->action_edit();
  }


  /**
   * AddHeader override for projectmenu.
   */
  public function addHeader()
  {
    $this->getPage()->register_style( atkTheme::getInstance()->stylePath( 'tabs.css' ) );
    return $this->getProjectMenu() . '<br/>';
  }


  /**
   * AdminHeader override for projectmenu.
   */
  public function viewHeader()
  {
    return $this->getProjectMenu() . '<br/>';
  }


  /**
   * Get the project menu.
   *
   * @return string HTML
   */
  private function getProjectMenu()
  {
    atkimport( 'module.project.projectmenu' );
    $pm = new projectmenu( 'project.projectdocument', $this->m_project );
    return $pm->getTabHtml();
  }

}
