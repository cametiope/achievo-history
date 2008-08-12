<?php

  /**
   * Relation has the same functionality as the normal manytoonerelation,
   * but the attribute itself is not stored in the node.
   *
   * @package modules
   * @subpackage project
   */

  userelation("atkmanytoonerelation");

  class packageManyToOneRelation extends atkManyToOneRelation
  {
    var $m_store = false;
    var $m_userid = "";

    function packageManyToOneRelation($name, $destination, $flags=0)
    {
      $this->m_userid = &atkGetUserId();
      parent::atkManyToOneRelation($name, $destination,$flags);
    }

    function setUserId($userid)
    {
      $this->m_userid = $userid;
    }

    function setStore($store=false)
    {
      $this->m_store = $store;
    }

    function storageType($mode)
    {
      if($this->m_store == false)
        return NOSTORE;
      else
        return parent::storageType($mode);
    }

    function dbFieldType()
    {
      if($this->m_store == false)
        return "";
      else
        return parent::dbFieldType();
    }

    function edit(&$record, $fieldprefix="", $mode="")
    {     
      if( $mode == 'add' ) // 'add' indicates that we're on the timeregistration page
      {
        $filter=$this->parseFilter($this->m_destinationFilter,$record);
        $newsel = $this->getHtmlId($fieldprefix);
  
        $selectionlink = "";
        if(!$this->hasFlag(AF_RELATION_AUTOCOMPLETE))
        {
          $selectionlink = "&nbsp;".$this->createSelectionLink($newsel,$filter);
        }
        return parent::edit($record,$fieldprefix,$mode).$selectionlink;
      }
      else // otherwise we're using the reports module
      {
        return parent::edit($record, $fieldprefix, $mode);
      }      
    }

    /**
     * Creates a link to the selection page of the projectnode.
     *
     */
    function createSelectionLink($id,$filter)
    {
      return $this->_getSelectLink($id, $filter);
    }

    /**
     * When the attribute is used as a non-storing attribute, we need to
     * set the join-condition with the package manually.
     * otherwise, we can use the normal join condition.
     *
     * @param object $query
     * @param String $tablename
     * @param String $fieldalias
     * @return String join condition
     */
    function getJoinCondition(&$query, $tablename="",$fieldalias="")
    {
      if($this->m_store == false)
        return $fieldalias.'.id = phaseid.master_id';        
     
      return parent::getJoinCondition($query,$tablename,$fieldalias);
    }
    

  }