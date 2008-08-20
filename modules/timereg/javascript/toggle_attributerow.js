function toggle_attributerow( id, destination )
{
  var project = ( $F( $(id) ) );
  var el      = $( 'ar_' + destination );

  new Ajax.Request('modules/timereg/project_uses_activities.php?project=' + project,
  {
    method:'get',
    onSuccess: function( result ) {
      if (result.responseText != '1') {
        el.style.display="none";
      }
      else {
        el.style.display="";
      }
    },
  }
  );  

  return true;     
}