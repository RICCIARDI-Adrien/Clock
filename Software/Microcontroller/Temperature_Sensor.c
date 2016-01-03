/** @file Temperature_Sensor.c
 * @see Temperature_Sensor.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Temperature_Sensor.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void TemperatureSensorInitialize(void)
{
	// Set only RA0 as analog
	trisa.0 = 1; // Configure pin as input

	// Configure the ADC module
	adcon1 = 0x8E; // Result of conversion is left justified, configure only RA0 as analog
	adcon0 = 0x41; // Conversion clock at Fosc / 8 (TAD = 2µs), select channel 0 (RA0), enable the ADC module
}