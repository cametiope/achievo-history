<?php

  class trafficLightUtils 
  {
    static public function getTrafficLightColor( $value )
    {
      if (is_null($value))
        return 'none';

      $red = atkConfig::get('project', 'trafficlight_red');
      if ( $value >= $red['start'] && $value <= $red['end'])
        return 'red';

      $yellow = atkConfig::get('project', 'trafficlight_yellow');
      if ( $value >= $yellow['start'] && $value <= $yellow['end'])
        return 'yellow';

      $green = atkConfig::get('project', 'trafficlight_green');
      if ( $value >= $green['start'] && $value <= $green['end'])
        return 'green';

      return 'none';      
    }      
  }