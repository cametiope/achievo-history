<?php
function clickableImages($id, $srcopen, $srcclose, $regopen, $regclose)
{ 
  $openimageid = "img_open_$id";
  $closeimageid = "img_close_$id";
  
  $hrefopen = "javascript:openObject('projects','$regopen','$openimageid','$closeimageid')";
  $hrefclose = "javascript:closeObject('projects','$regclose','$openimageid','$closeimageid')";
  
  $imgopen  = '<img id="'.$openimageid.'" src="'.$srcopen.'" border="0">';
  $imgclose = '<img id="'.$closeimageid.'" style="display:none" src="'.$srcclose.'" border="0">';

  $images = "<a href=\"$hrefopen\">$imgopen</a>";
  $images.= "<a href=\"$hrefclose\">$imgclose</a>";
  return $images;    
}
?>