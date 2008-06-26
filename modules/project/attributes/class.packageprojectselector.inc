<?php

class packageProjectSelector extends atkListAttribute
{

  /**
   * Constructor
   * @param $name name of the attribute
   * @param $flags flags for this attribute
   */
  function packageProjectSelector($name, $optionArray = array(), $valueArray = array(), $flags=0, $size=0)
  {
    if( false !== ( $options = $this->getOptionArray() ) )
    {
      $this->atkListAttribute($name, $options['optionArray'], $options['valueArray'], AF_FORCE_LOAD|AF_HIDE_LIST, $size=0);
    }
  }
  
  
 /**
  * Returns a piece of html code that can be used in a form to edit this
  * attribute's value.
  * 
  * Note that master and master_id should be present in the node with hidden values
  * 
  * @param $record Array with fields
  * @return piece of html code with a checkbox
  */
  public function edit($record="", $fieldprefix="")
  {
    $id = $this->getHtmlId($fieldprefix);

    $result  = '<select id="'.$id.'" name="'.$id.'"  class="atklistattribute" '.$onchange.($this->m_width?" style='width: {$this->m_width}px'":"").'>';

    $values   = $this->getValues($record);
    $recvalue = atkArrayNvl( $record, $this->fieldName() );
    $this->m_selected = ( false !== strpos( $record['master'], 'pack') ) ? 'packages' : 'projects';
    $this->m_selected .=  '_' . $record['master_id'];   
    
    $prev_optgroup = '';
    for ($i=0;$i<count($values);$i++)
    {      
      // If the current value is selected or occurs in the record
      if ((!is_null($this->m_selected) && $values[$i] == $this->m_selected) ||
          ( is_null($this->m_selected) && $values[$i] == $recvalue && $recvalue!==""))
      {
        $sel = "selected";
      }
      else
      {
        $sel = "";
      }
      
      // Todo: optgroup can be handled a bit cleaner (try to get rid of substr)
      $optgroup = ucfirst(substr($values[$i], 0, ( strpos($values[$i], '_') ) ) ); 
      if( $prev_optgroup != $optgroup )
      {
        if( $prev_optgroup != '' )
        {
          $result .= '</optgroup>';
        }
        $result .= '<optgroup label="'.$optgroup.'">';
        $result.= '<option value="'.$values[$i].'" '.$sel.'>'.$this->_translateValue($values[$i],$record);
        $prev_optgroup = $optgroup;        
      }
      else 
      {
        $result.= '<option value="'.$values[$i].'" '.$sel.'>'.$this->_translateValue($values[$i],$record);      
      }
    }
    $result .= '</optgroup>';
    $result.='</select>';
    return $result;
  }

  
  public function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
  {
    if( $mode == 'add' || $mode == 'update' )
    {
      if( !empty( $rec['project_package'] ) )
      {
        $master    = explode( '_', $rec['project_package'] );
        $master[0] = (false !== strpos( $master[0], 'project' ) ) ? 'project.project' : 'project.package';
        
        $query->addField( 'master', $master[0] );
        $query->addField( 'master_id', $master[1] );
      }
    }
  }

  
  /**
   * Fetch projects and packages from DB
   *
   * @return mixed
   */
  private function getOptionArray( )
  {
    global $ATK_VARS;
    
    $db = &atkGetDb();
    
    preg_match('/\'([0-9]*)\'/', $ATK_VARS['atkfilter'], $matches);
    $id = $matches[1];

    if( !$id )
    {
      return false;
    }
    
    $projects = $db->getRows( 'SELECT "projects_" AS prefix, id, name FROM project WHERE project.id = '.$id);
    $packages = $db->getRows( 'SELECT "packages_" AS prefix, id, name FROM package WHERE package.projectid = '.$id);

    $return  = array();
    $results = array_merge( $projects, $packages );
    if( count( $results ) > 0 )
    {
      foreach( $results AS $key => $val )
      {
        $return['valueArray'][]  = $val['prefix'] . $val['id'];
        $return['optionArray'][] = $val['name'];
      }
      return $return;
    }    
    return false;  
  }
    
}