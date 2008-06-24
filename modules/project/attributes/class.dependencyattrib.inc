<?php

useattrib("atklistattribute");

class dependencyAttribute extends atkAttribute
{
  /**
   * Constructor
   * @param $name Name of the attribute
   * @param $flags Flags for this attribute
   */
  function __construct($name, $flags=0)
  {
    /* base class constructor */
    $this->atkAttribute($name, $flags);
  }

  function fetchValue($postvars)
  {
    atk_var_dump($postvars);
    $values = isset($postvars[$this->formName()]) ? $postvars[$this->formName()] : null;
    $res = array();
    if (is_array($values))
    {
      if($values["first"]!=""&&$values["last"]!=""&&$values["first"]!=$values["last"])
      {
        $value = explode('_',$values['last']);
        $res["first"]=$values["first"];
        $res["last"]=$value[0];
        $res['dep_type'] = $value[1];
      }
      // Find out if we need to delete dependencies..
      while (list($name,$val) = each($values))
      {
        if (substr($name,0,3)=="del")
        {
          list($del,$first,$last, $dep_type) = split('_',$name);
          $delarr[] = array("first"=>$first,"last"=>$last,"dep_type"=>$dep_type);
        }
      }
      $res["del"] = $delarr;
    }
    return $res;
  }

  /**
   * Returns a piece of html code that can be used in a form to edit this
   * attribute's value.
   * @param $record array with fields
   * @return piece of html code with a textarea
   */
  function edit($record="")
  {
    $deps = $record[$this->fieldname()]["deps"];

    if (count($deps)==0)
    {
      // Someone could have pressed save, and we don't post records in this
      // attribute, so we have to reload them from the database
      $rows = $this->load("",$record);
      $deps = $rows["deps"];
    }

    $result = "<br>";

    if (count($deps)>0)
    {
      $data = array();
      $data[] = array(atktext('dependencies'),atktext('delete'));
      for ($i=0;$i<count($deps);$i++)
      {
        $label  = atktext($deps[$i]["dep_type"])." '".$deps[$i]["firstname"]."' ".atktext("mustbecompletedfirst");
        $delete = '&nbsp;&nbsp;<input type="checkbox" '.$this->getCSSClassAttribute("atkcheckbox").' name="'.$this->formName()."[del_".$deps[$i]["first"]."_".$deps[$i]["last"]."_".$deps[$i]["dep_type"].']" value="1">';
        $data[] = array($label,$delete);
      }
      $tbl = &atknew("atk.utils.atktablerenderer");
      $result.=$tbl->render($data,TBL_HEADER|TBL_ALTERNATE,"recordlist")."<br>";
    }

    $recs = $this->getPhasesAndPackages($record["projectid"], $record["id"]);

    $current_phase = '<input type="hidden" name="'.$this->formName().'[first]'.'" value="'.$record['id'].'" />';

    $result  .= atktext("addnewdependency").":<br>";            
    $result  .= atktext("phase")." ".$current_phase." ".atktext("mustbecompletedbefore");
    $result .= $this->createDropdownList( $recs );
    
    return $result;
  }

  
  private function createDropdownList($recs)
  {
    $id = $this->getHtmlId($fieldprefix);
    $result  = '<select id="'.$id.'" name="'.$id.'[last]"  class="atklistattribute" '.$onchange.($this->m_width?" style='width: {$this->m_width}px'":"").'>';
    $result .= '<option value="">---</option>';
    $prev_optgroup = '';

    for ($i=0;$i<count($recs);$i++)
    {           
      // Todo: optgroup can be handled a bit cleaner (try to get rid of substr)
      $optgroup = ucfirst($recs[$i]['dep_type']); 
      if( $prev_optgroup != $optgroup )
      {
        if( $prev_optgroup != '' )
        {
          $result .= '</optgroup>';
        }
        $result .= '<optgroup label="'.$optgroup.'">';
        $result.= '<option value="'.$recs[$i]['id'].'_'.$recs[$i]['dep_type'].'">'.$recs[$i]['name'];
        $prev_optgroup = $optgroup;        
      }
      else 
      {
        $result.= '<option value="'.$recs[$i]['id'].'_'.$recs[$i]['dep_type'].'">'.$recs[$i]['name'];      
      }
    }
    $result .= '</optgroup>';
    $result.='</select>';    
   
    return $result;
  }
  
  function getPhasesAndPackages($projectid,$phase_id)
  {
    if (is_null($projectid) || is_null($phase_id) )
    {
      atkerror("dependancyattrib->getPhasesAndPackages: Empty projectid given");
      return array();
    }

    $db = &atkGetDb();

    $phases   = 'SELECT id, name, "phase" AS dep_type    FROM phase WHERE phase.projectid = '.$projectid.' AND phase.id <> '.$phase_id.' AND phase.id NOT IN (SELECT phaseid_col FROM dependency WHERE dep_type = "phase" AND phaseid_row = '.$phase_id.' ) ORDER BY phase.name';
    $packages = 'SELECT id, name, "package" AS dep_type FROM package WHERE package.projectid = '.$projectid.' AND package.id NOT IN(SELECT phaseid_col FROM dependency WHERE dep_type = "package" AND phaseid_row = '.$phase_id.') ORDER BY package.name';
    
    return array_merge($db->getrows($phases), $db->getrows($packages));
  }

  /**
   * We don't support searching
   * @param $record array with fields
   * @return search field
   */
  function search($record="")
  {
    return "&nbsp;";
  }

  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
  {
     // we don't add anything to the query.. (we have our own load and store methods
  }

  function load($notused, $rec)
  {
    if ($rec["id"]!="")
    {
      $query = 'SELECT 
                  dependency.phaseid_col AS `first`, phase.name AS firstname, dependency.phaseid_row AS last, dependency.dep_type
                FROM 
                  dependency, phase
                WHERE 
                  phase.id = dependency.phaseid_col
                AND
                  dependency.phaseid_row = '. (int) $rec["id"].'
                ORDER BY 
                  dependency.phaseid_col';

      $db = &atkGetDb();
      return array("deps"=>$db->getrows($query));
    }
    else
    {
      return array("deps"=>array());
    }
  }

  function store($notused, $rec)
  {
    $db = &atkGetDb();

    $values = $rec[$this->fieldName()];

    // First delete deleted items.
    $delarr = $values["del"];

    if (is_array($delarr))
    {
      for ($i=0;$i<count($delarr);$i++)
      {        
        $delquery = "DELETE
                     FROM
                       dependency
                     WHERE
                       phaseid_row = '".$delarr[$i]["last"]."'
                     AND 
                       phaseid_col = '".$delarr[$i]["first"]."'
                     AND
                       dep_type = '".$delarr[$i]["dep_type"]."'";
        
        $db->query($delquery);
      }
    }

    // Now add an new item..
    if ($values["first"]!="" && $values["last"]!="")
    {
      $query = "INSERT
                  INTO dependency (phaseid_row,phaseid_col,dep_type)
                  VALUES ('".$values["first"]."','".$values["last"]."', '".$values['dep_type']."')";

      $db->query($query);
    }
    return true;
  }

  function getSearchModes()
  {
    // search not supported.
    return array();
  }

  function searchCondition(&$query, $table, $value, $searchmode)
  {
    // Search not supported.
  }

  function dbFieldType()
  {
    return "";
  }
}