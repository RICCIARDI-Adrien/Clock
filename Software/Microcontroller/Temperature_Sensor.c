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
	adcon1 = 0x8E; // Result of conversion is right justified, configure only RA0 as analog
	adcon0 = 0x41; // Conversion clock at Fosc / 8 (TAD = 2µs), select channel 0 (RA0), enable the ADC module
}

unsigned char TemperatureSensorGetTemperature(void)
{
	unsigned long Double_Word;

	// Start conversion
	adcon0.GO = 1;
	while (adcon0.GO); // Wait for the conversion to finish
	
	// Convert the raw voltage to a centigrade temperature
	Double_Word = (adresh << 8) | adresl; // Get the raw ADC value
	Double_Word *= 100; // Multiply by 100 to perform fixed point calculations (use 100 because the sensor conversion is 10mv/°C, so 1°C = 0.01V
	Double_Word = (500 * Double_Word) / 102300; // Convert the raw ADC value to volts*100 using the following formula : Voltage * 100 = 500 * (Raw_ADC_Value * 100) / 102300
	
	return (unsigned char) Double_Word;
}