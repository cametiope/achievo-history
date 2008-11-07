<?php
   userelation("atkonetomanyrelation");

  /**
    * oneToManyMemoRelation relation.
    *
    * This relation overrides some of atkOneToManyRelation's methods, to be able to alter
    * appearance and the likes
    */
  class oneToManyMemoRelation extends atkOneToManyRelation
  {        
    
    /**
     * Override parent constructor
     *
     */
    public function __construct( $name, $destination, $refKey="", $flags=0 )
    {
      parent::atkOneToManyRelation($name, $destination, $refKey, AF_FORCE_LOAD|$flags );
    }
    
    
    /**
     * Override display function to show the same field as in edit mode
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     * @return string
     */
    public function display( $record="", $fieldprefix="", $mode="")
    {
      return $this->edit( $record, $fieldprefix, $mode);
    }
    
    
    /**
     * Override edit method: we want to have the results appear in a textfield;
     * and not as a recordlist
     *
     * @param array $record
     * @return string
     */
    public function edit( $record="", $fieldprefix="", $mode='')
    {      
      $postvars = atkGetPostVar();
      
      $master    = array_key_exists( 'atknodetype', $postvars ) ? $postvars['atknodetype'] : 'project.project';
      
      $project = decodeKeyValuePair( sessionLoad('projectid') );
      sort( $project ); // make sure the value we need is at index 0
      $project_id = $project[0];
      
      // generate where clause based on atknodetype
      $where = '';
      switch( $master )
      {
        case 'project.project':
          $where = ' memo.project_id = '.$project_id.' AND memo.master = "project.project" AND memo.updatestyle = "m" ';
          break;
          
        case 'project.package':
          $where .= ' ( memo.master = "project.package" AND memo.master_id = '.$record['id'].' AND memo.updatestyle = "m" ) 
                        OR
                      ( memo.project_id = '.$project_id.' AND memo.updatestyle = "a" )';
          
          $db = atkGetDb();
          $phases = $db->getRows( 'SELECT id FROM phase WHERE master = "project.package" AND master_id = '.$record['id'] );
          if( count( $phases ) > 0 )
          {            
            $tasks = array();
            foreach( $phases AS $task )
            {
              $tasks[] = $task['id'];
            }
            $where .= 'OR ( memo.master="project.phase" AND memo.master_id IN ( '.implode(', ', $tasks ).' ) )';
          }                    
          break;
          
        case 'project.phase':
          $where .= ' ( memo.master = "project.phase" AND memo.master_id = '.$record['id'].' ) ';
          break;
          
        default:
          $where .= ' memo.project_id = '.$project_id.' ';
          break;
      }

      if( (int) ( $project_id ) > 0 )
      {
        $db = atkGetDb();
        $rows = $db->getRows( '
          SELECT DATE_FORMAT(date_added, "%d-%m-%Y %H:%i") AS date_added_formatted, user, location, memo 
          FROM memo 
          WHERE 
            '.$where.'
          ORDER BY date_added DESC' ); 
  
        $text = $this->formatText( $rows );        
        $id   = 'readonly_memofield';
        
        $result= "<textarea id='$id' name='$id' ";
        $result.= "disabled='disabled' ";
        $result.= "rows='10' ";
        $result.= "cols='60' ";
        $result.=">\n".htmlspecialchars($text)."</textarea>";
        return $result;
      }
                     
      return parent::edit( $record, $fieldprefix, $mode );
    }
  
    
    /**
     * Format the recordset as if it were plain text
     *
     * @param array $rows
     * @return string
     */
    private function formatText( $rows )
    {
      if( count( $rows ) == 0 )
      {
        return '';
      }
      
      $text = '';
      foreach( $rows AS $row )
      {
        $text .= $row['date_added_formatted'] .' >> '. $row['user'] .' >> '. $row['location'] ."\r\n\r\n";
        $text .= htmlentities( $row['memo'] )."\r\n\r\n\r\n";
      }
      
      return $text;
    }
  }