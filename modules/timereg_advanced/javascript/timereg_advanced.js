var Timereg_Advanced = {
  
  completed_time : 0,
  completed_remark: 0,
  
  updateCheckbox : function ( id ) {
    this.validateTimeregTime(id);
    this.validateTimeregRemark(id);
    
    field = 'cb_'+id;
    if (this.completed_time == 1 && this.completed_remark == 1) $(field).checked = 1;
    else $(field).checked = 0;
  },
  
  validateTimeregTime : function( id ) {
    h = 'timereg['+id+'][time][hours]';
    m = 'timereg['+id+'][time][minutes]';
    this.completed_time = ($(h).value == 0 && $(m).value == 0) ? 0 : 1;
  }, 
  
  validateTimeregRemark : function( id ) {
    r = 'timereg['+id+'][remark]';
    this.completed_remark = ($(r).value == '') ? 0 : 1;
  },
  
  removeFavorite : function( url, phase ) {
    new Ajax.Request(url,
    {
      method:'get',
      onSuccess: function(request){
        row = 'r1_'+phase;
        $(row).style.display = 'none';
      },
      asynchronous: false,
      onFailure: function(){}
    });
  }
  
}