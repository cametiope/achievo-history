<?php

  define('IMG_TRAFFIC_GREEN',   'images/traffic_green.png');
  define('IMG_TRAFFIC_YELLOW',  'images/traffic_yellow.png');
  define('IMG_TRAFFIC_RED',     'images/traffic_red.png');
  define('IMG_TRAFFIC_UNKNOWN', 'images/traffic_unknown.png');


  class trafficLightAttribute extends atkAttribute
  {

  	/**
  	 * Constructor
  	 *
  	 * @param String $name
  	 * @param Integer $flags
  	 */
    public function __construct($name, $flags=0)
    {
      $this->atkAttribute($name, $flags|AF_READONLY|AF_HIDE_ADD);
    }

    
    /**
     * Display override for traffic light
     *
     * @param Array $record
     * @param String $mode
     * @return String html
     */
    public function display( $record, $mode = '')
    {
      return $this->getTrafficLight($record, $mode);
    }

    
    /**
     * Ensure the attribute will be updated
     *
     * @return unknown
     */
    public function needsUpdate()
    {
      return true;
    }
    
    
    /**
     * Generate trafficlight based on configuration options
     *
     * @param Array $record
     * @param String $mode
     * @return String html
     */
    private function getTrafficLight($record, $mode)
    {
    	$value = $record[$this->fieldName()];

    	if (is_null($value))
    	  return '<img src="'.IMG_TRAFFIC_UNKNOWN.'" alt="'.atktext('trafficlight_unknown').'" />';

    	$red = atkConfig::get('project', 'trafficlight_red');
      if ( $value >= $red['start'] && $value <= $red['end'])
        return '<img src="'.IMG_TRAFFIC_RED.'" alt="'.atktext('trafficlight_red').'" />';

      $yellow = atkConfig::get('project', 'trafficlight_yellow');
      if ( $value >= $yellow['start'] && $value <= $yellow['end'])
        return '<img src="'.IMG_TRAFFIC_YELLOW.'" alt="'.atktext('trafficlight_yellow').'" />';

      $green = atkConfig::get('project', 'trafficlight_green');
      if ( $value >= $green['start'] && $value <= $green['end'])
        return '<img src="'.IMG_TRAFFIC_GREEN.'" alt="'.atktext('trafficlight_green').'" />';

      return '<img src="'.IMG_TRAFFIC_UNKNOWN.'" alt="'.atktext('trafficlight_unknown').'" />';
    }

  }