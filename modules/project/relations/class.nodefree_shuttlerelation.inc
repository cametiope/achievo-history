<?php
   /** @internal base include */
   userelation("atkshuttlerelation");

  /**
    * Nodefree_ShuttleRelation relation.
    *
    * The relation shows a shuttlerelation, which does not map to nodes.
    * This is used to show a selectlist with which results can be filtered from a searchform.
    */
  class Nodefree_ShuttleRelation extends atkShuttleRelation
  {
    
    
    var $m_recordset;
        
    
    /**
     * Override parent constructor
     *
     */
    public function __construct( $recordset, $attribute = 'packages' )
    {
      $this->m_name = $attribute;
      $this->m_recordset = $recordset;
    }
        
    
    /**
     * Return a piece of html code to edit the attribute
     * @param $record Current record
     * @return String piece of html code
     */
    function edit( $record )
    {
      $selectedPk = array();
            
      $recordset = $this->m_recordset;                    
      $left  = array();        
      $right = array();
      $width = 100;
      
      // Preset selected entries from 
      for( $i = 0; $i < count( $this->m_recordset ); $i++ )
      {
        if( is_array( $record[ $this->m_name ] ) && in_array( $this->m_recordset[$i]['id'], $record[ $this->m_name ] ) ) 
        {
          $right[] = array( 'id' => $this->m_recordset[$i]['id'], 'name' => $this->m_recordset[$i]['name'] );
        }
        else 
        {
          $left[]  = array( 'id' => $this->m_recordset[$i]['id'], 'name' => $this->m_recordset[$i]['name'] );
        }
      }

      $result = '<table border="0"><tr><td>'.atktext('available', 'atk').':<br/>';      
      $leftname  = $this->m_name .'_sel';
      $rightname = $this->m_name;            
      $result.= $this->_renderSelect($leftname, $left, $width, $rightname);
      $result.= '</td><td valign="center" align="center">';
      $result.= '<input type="button" value="&gt;" onClick="shuttle_move(\''.$leftname.'\', \''.$rightname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;" onClick="shuttle_move(\''.$rightname.'\', \''.$leftname.'\');"><br/><br/>';
      $result.= '<input type="button" value="&gt;&gt;" onClick="shuttle_moveall(\''.$leftname.'\', \''.$rightname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;&lt;" onClick="shuttle_moveall(\''.$rightname.'\', \''.$leftname.'\');">';
      $result.= '</td><td>'.atktext('selected', 'atk').':<br/>';
      $result.= $this->_renderSelect($rightname, $right, $width, $leftname);
      
      $page = atkinstance('atk.ui.atkpage');
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkshuttlerelation.js");
      $page->register_submitscript("shuttle_selectAll('".$rightname."');");
      
      $result.= '</table>';

      return $result;
    }

    /**
     * Render the multiselect list control
     * @access private
     * @param String $name The name of the list control
     * @param array $recordset The list of records to render in the control
     * @param int $width The width of the control in pixels
     * @param String $opposite The name of the list control connected to this list control for shuttle actions
     * @return String piece of html code
     */
    function _renderSelect($name, $recordset, $width, $opposite)
    {
      $result = '<select class="shuttle_select" id="'.$name.'" name="'.$name.'[]" multiple size="10" style="width: '.$width.'px;" onDblClick="shuttle_move(\''.$name.'\', \''.$opposite.'\')">';
      for($i=0, $_i=count($recordset); $i<$_i; $i++)
      {
        $result.= '<option value="'.$recordset[$i]['id'].'">'.$recordset[$i]['name'];
      }
      $result.= '</select>';
      return $result;
    }   

  }