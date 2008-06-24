<?php
   
  class memoAttribute extends atkTextAttribute
  {
    
    function memoAttribute($flags=0)
    {
      $this->atkTextAttribute("project_memo", $flags);
    }    
    
    
    
    /**
     * Adds slashes and a timestamp prefix
     * 
     * @todo   Ruud: sanitize postvars before sending the info to the DB (possible leak)
     * @param  $rec array with values
     * @return escaped string
     */
    function value2db($rec)
    {
      if( $rec[$this->fieldName()] == '' )
      { 
        return atkGetPostVar( 'memo_previous_content' ); 
      }
      else
      {
        $db   = &$this->getDb();
        $user = getUser();
       
        // Prepend timestamp and userID
        $content = date('Y-m-d H:i')." >> ". $user['userid'] ."\r\n\r\n";      
        
        if ($db->getType() != "oci9" || $this->dbFieldType() != 'text')
        { 
          $content .= $db->escapeSQL($rec[$this->fieldName()]);
        }
        else
        {
          $content .= $rec[$this->fieldName()]; // CLOB in oci9 don't need quotes to be escaped EVIL HACK! THIS IS NOT ATKTEXTATTRIBUTE's PROBLEM!
        }
      }
      // Add original content from post array
      return $content .= "\r\n\r\n\r\n" . atkGetPostVar( 'memo_previous_content' );
    }
    
    
    
    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * 
     * @param $record array with fields
     * @param $size array with field rows and cols for additional size of the text box
     * @return piece of html code with a textarea
     */
    function edit( $record="", $fieldprefix="", $mode='add' )
    {      
      $text = (isset($record[$this->fieldName()]) && $mode != 'edit') ? $record[$this->fieldName()] : '';
      $text_previous = (isset($record[$this->fieldName()]))           ? $record[$this->fieldName()] : '';      
      $cols = ($this->m_cols != 0)                                    ? $this->m_cols : $this->maxInputSize();
      $rows = $this->m_rows; 
      
      $id = $fieldprefix.$this->fieldName();
      $this->registerKeyListener($id, KB_CTRLCURSOR);
      if ($rows =="" || $rows == 0)
      {
        $rows = 10;
      }
      
      // Watch out, $maxsize isn't supported for a textarea.
      if ($this->m_autoadjust)
      {
        $this->doAutoAdjust(htmlspecialchars($text), $rows, $cols);
      }

      $result = "<textarea id='$id' name='$id' wrap='".$this->getWrapMode()."' ";
      if ($rows) $result.= "rows='$rows' ";
      if ($cols) $result.= "cols='$cols' ";
      $result.=$this->getCSSClassAttribute();
      $result.=">\n".htmlspecialchars($text)."</textarea>";
      // Add previous content of the memo in an hidden input field
      $result.="\n<input type='hidden' name='memo_previous_content' value='".$text_previous."' />";
      return $result;
    }
  }