<?php
  define('IMG_TRAFFIC_GREEN',   'images/traffic_green.gif');
  define('IMG_TRAFFIC_YELLOW',  'images/traffic_yellow.gif');
  define('IMG_TRAFFIC_RED',     'images/traffic_red.gif');
  
   
  class trafficLightAttribute extends atkAttribute
  {
    
    function trafficLightAttribute($flags=0)
    {
      $this->atkAttribute("traffic_light", $flags|AF_READONLY|AF_HIDE_ADD);
    }
    

    function display( $record, $mode = '')
    {
      switch( $record['traffic_light'] )
      {
        case 1:
          return '<img src="'.IMG_TRAFFIC_RED.'" alt="'.atktext('traffic_red_light').'" />';
        break;
        case 2:
          return '<img src="'.IMG_TRAFFIC_YELLOW.'" alt="'.atktext('traffic_red_light').'" />';      
        break;
        case 3: 
          return '<img src="'.IMG_TRAFFIC_GREEN.'" alt="'.atktext('traffic_red_light').'" />';      
        break;       
      }
      return '';
    }
      
  }