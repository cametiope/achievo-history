var Timereg_Advanced = {
  
  addFavorite : function( url, phase ) {
    new Ajax.Request(url,
    {
      method:'get',
      onSuccess: function(request){
        icon = 'addicon_'+phase;
        $(icon).style.display = 'none';
      },
      asynchronous: false,
      onFailure: function(){}
    });
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
