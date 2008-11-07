<?php

  function pimAllocations()
  {
    $node = &getNode("project.pim_allocation");    
    $recs = $node->selectDb();

    if (count( $recs ) > 0 )
    {
      $recordlist = &atknew("atk.recordlist.atkrecordlist");
      $html = $recordlist->render($node, $recs, array());
      
      $res = '<table border="0">';
      $res.= '<tr>';
      $res.= '<td valign="top">'.atktext('allocations', 'project').'</td>'; 
      $res.= '</tr><tr><td>&nbsp;</td></tr><tr>';
      $res.= '<td valign="top">'.$recordlist->render($node, $recs, array()).'</td>';
      $res.= '</tr></table>';
      return $res;
    }    
    return atktext('allocation_no_items');
  }