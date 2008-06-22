<?php

/**
 * This relation take possible package node have relation with two nodes - package and project
 *
 * @package achievo
 * @subpackage modules
 *
 * @author yury <yury@achievo.org>
 *
 * @copyright (c) 2008 Ibuildings.nl BV
 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
 *
 * @version $Revision$
 * $Id$
 */
userelation("atkmanytoonerelation");

class dynamicManyToOneRelation extends atkManyToOneRelation
{
  var $m_linkType = array();
  var $m_cache = array();

  function __construct($name, $flag=0, $link_type = "master")
  {
    $this->m_linkType = $link_type;
    $this->atkManyToOneRelation($name, "dummynode", $flag| AF_MANYTOONE_LAZY);
  }

  function partial_autocomplete($mode)
  {
    $this->_fixDestination(atkArrayNvl($this->m_ownerInstance->m_postvars,$this->m_linkType));
    return parent::partial_autocomplete($mode);
  }

  function createDestination($record='')
  {
    $this->_fixDestination(atkArrayNvl($record,$this->m_linkType));
    if($this->m_destination == "dummynode") return false;
    else return parent::createDestination();

  }

  function _fixDestination($type)
  {
    if(isset($type) && $this->m_destination != $type)
    {
      $this->m_destination = $type;
      if(is_object($this->m_cache[$type])) $this->m_destInstance = $this->m_cache[$type];
      else
      {
        $this->m_destInstance = "";
        parent::createDestination();
        $this->m_cache[$type] = $this->m_destInstance;
      }
    }
  }

  function getSelectedRecord($record=array(), $mode="select")
  {
    $this->_fixDestination(atkArrayNvl($record,$this->m_linkType));
    return parent::getSelectedRecord($record, $mode);
  }

  function db2value($record)
  {
    $this->_fixDestination(atkArrayNvl($record,$this->m_linkType));
    return parent::db2value($record);
  }

  function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
  {
    $this->_fixDestination(atkArrayNvl($defaults,$this->m_linkType));
    return parent::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);
  }

  function populate(&$record, $fullOrFields=false)
  {
    $this->_fixDestination(atkArrayNvl($record,$this->m_linkType));
    return parent::populate($record, $fullOrFields);
  }

  function fetchValue($postvars)
  {
    $this->_fixDestination(atkArrayNvl($postvars,$this->m_linkType));
    return parent::fetchValue($postvars);
  }

  function display($record, $mode='list')
  {
    $this->_fixDestination(atkArrayNvl($record,$this->m_linkType));

    if($this->m_destination == "dummynode") return '';
    return parent::display($record, $mode);
  }

  function partial_refresh($mode)
  {
    $record = $this->m_ownerInstance->updateRecord();

    $this->_fixDestination(atkArrayNvl($record,$this->m_linkType));

    $fieldprefix = $this->m_ownerInstance->m_postvars['atkfieldprefix'];
    $this->_registerDependees($mode, $fieldprefix);

    $arr = array('fields' => array());
    $defaults = &$record;
    $error = array();

    $this->addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);

    $script = '';
    atkimport('atk.utils.atkjson');
    foreach ($arr['fields'] as $field)
    {
      //atkJSON::encode expect string in in ASCII or UTF-8 format, so convert data to UTF-8
      $value = atk_iconv(atktext("charset"),"UTF-8",$field['html']);
      $script .= "\$('".str_replace('.', '_', $this->m_ownerInstance->atkNodeType().'_'.$field['id'])."').innerHTML = ".atkJSON::encode($value).";\r\n";
    }

    $this->_registerDependees($mode, $fieldprefix);

    return '<script type="text/javascript">'.$script.'</script>';
  }

}

?>