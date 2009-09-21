<?php

useattrib( 'atkDurationAttribute' );

/**
 * The atkRemainingEffortTimeAttribute is an attribute for entering a length of time,
 * but unlike the atkDurationAttribute a 0 value is allowed.
 *
 * @author Danny Peeters <danny@ibuildings.nl>
 * @package achievo
 * @subpackage timereg
 */
class remainingEffortTimeAttribute extends atkDurationAttribute
{


  /**
   * Constructor.
   *
   * @param string $name
   * @param string $resolution
   * @param string $maxtime
   * @param int $flags Bitmask of node flags (NF_*)
   * @param int $size
   */
  public function __construct( $name, $resolution = "1m", $maxtime = "10h", $flags = 0, $size = 0 )
  {
    parent::atkDurationAttribute( $name, $resolution, $maxtime, $flags, $size );
  }


  /**
   * Check if a record has an empty value for this attribute.
   * @param array $record The record that holds this attribute's value.
   * @return boolean
   */
  public function isEmpty( $record )
  {
    return (! isset( $record[$this->fieldName()] ) || $record[$this->fieldName()] === "" || $record[$this->fieldName()] === NULL);
  }

}