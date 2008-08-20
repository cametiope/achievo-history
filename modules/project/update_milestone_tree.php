<?php

  $config_atkroot = "./../../";
  include_once($config_atkroot."atk.inc");
  atkimport('module.project.deliverable');
  
  atksession();
  $vars    = &atkGetPostVar();
  $session = &atkSessionManager::getSession();
  $milestone = &atkGetNode('deliverable');
    
  
  /**
   * Return generated HTML for packages
   *
   * @param array $packages
   * @param integer $depth
   * @return string
   */
  function paintPackages( $packages, $depth )      
  { 
    $html = '';
    if( count( $packages ) > 0 )
    {
      foreach( $packages AS $package )
      {     
        $html .= '<div style="margin-bottom: 5px; background-color: #e7ebef; position: relative; float: left; width: 100%;">';         
        $html .= '<div style="width: '.( 350 - ( ( $depth + 1 ) * 20 ) ).'px; position: relative; float: left; padding-left: '.( ( $depth + 1 ) * 20 ).'px;">';
        $html .=  '<img src="images/plus.gif" id="img_fold_pa_'.$package['id'].'" alt="unfold" onclick="toggle_tree(\'package_'.$package['id'].'\', \'img_fold_pa_'.$package['id'].'\'); return false;" />';
        $html .=   '<img src="images/package.gif" alt="package" />';
        $html .=   $package['name'];
        $html .= '</div>';
        $html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
        $html .= '<div style="width: 120px; position: relative; float: left">'.$package['progress'].'</div>';
        $html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
        $html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
        $html .= '<div style="width: 60px; position: relative; float: left">&nbsp;</div>';
        $html .= '</div>';
        $html .= '<div id="package_'.$package["id"].'"></div>';
      }      
    }
    return $html;    
  } 

  
  /**
   * Return generated HTML for phases
   *
   * @param array $phases
   * @param integer $depth
   * @return string
   */ 
  function paintPhases( $phases, $depth )
  {
    $html = '';
    if( count( $phases ) > 0 )
    {    
      foreach( $phases AS $phase )
      {
        $html .= '<div style="margin-bottom: 5px; background-color: #e7ebef; position: relative; float: left; width: 100%;">';         
      	$html .= '<div style="width: '.( 350 - ( ( $depth + 1 ) * 20 ) ).'px; position: relative; float: left; padding-left: '.( ( $depth + 1 ) * 20 ).'px;">';
        $html .=   '<img src="images/phase.gif" alt="phase" />';
      	$html .=   $phase['name'];
      	$html .= '</div>';
      	$html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
      	$html .= '<div style="width: 120px; position: relative; float: left">'.$phase['progress'].'</div>';
      	$html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
      	$html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
      	$html .= '<div style="width: 60px; position: relative; float: left">&nbsp;</div>';
        $html .= '</div>';            
      }
    }
    
    return $html;
  }
  
  
  /**
   * Return generated HTML for documents
   *
   * @param array $documents
   * @return string
   */ 
  function paintDocuments( $documents )
  {
    $html = '';
    if( count( $documents ) > 0 )
    {    
      foreach( $documents AS $document )
      {
        $html .= '<div style="margin-bottom: 5px; background-color: #e7ebef; position: relative; float: left; width: 100%;">';         
      	$html .= '<div style="width: 350px; position: relative; float: left; padding-left: 20px;">';
        $html .=   '<img src="images/document.gif" alt="document" /> ';      	
     	  $html .=   $document['name'];
      	$html .= '</div>';
      	$html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
      	$html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
      	$html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
      	$html .= '<div style="width: 120px; position: relative; float: left">&nbsp;</div>';
      	$html .= '<div style="width: 60px; position: relative; float: left">&nbsp;</div>';
        $html .= '</div>';            
      }
    }
    
    return $html;
  }
    
  
  // Ajax called, fetch and print children of a milestone
  if( array_key_exists( 'milestone', $vars ) )
  {
    $children = $milestone->getMilestoneChildren( $vars['milestone'] );
    
    echo paintPackages( $children['packages'], 0 ) .
         paintPhases( $children['phases'], -1 ) .
         paintDocuments( $children['documents'] );
    exit;
  }
  
  // Ajax called, fetch and print children of a package  
  if( array_key_exists( 'package', $vars ) )
  {
    $children = $milestone->getPackageChildren( $vars['package'] );

    echo paintPackages( $children['packages'], $vars['depth'] ) .
         paintPhases( $children['phases'], $vars['depth'] );
    exit;
  }