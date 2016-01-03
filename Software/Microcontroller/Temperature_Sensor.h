/** @file Temperature_Sensor.h
 * Allow to sample the LM35DZ sensor connected to RA0.
 * @author Adrien RICCIARDI
 */
#ifndef H_TEMPERATURE_SENSOR_H
#define H_TEMPERATURE_SENSOR_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the ADC module and the pin RA0 as analog. All other port A pins are digital. */
void TemperatureSensorInitialize(void);

/** Get the current temperature converted to centigrade.
 * @return The current temperature converted to centigrade. The temperature will always be positive.
 */
unsigned char TemperatureSensorGetTemperature(void);

#endif