/** @file Main.c
 * Clock main loop.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Button.h"
#include "Display.h"
#include "Ring.h"
#include "RTC.h"
#include "Temperature_Sensor.h"
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Microcontroller configuration
//--------------------------------------------------------------------------------------------------
// Microcontroller fuses
#pragma DATA _CONFIG, _CP_OFF & _DEBUG_OFF & _WRT_ENABLE_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _PWRTE_ON & _WDT_OFF & _XT_OSC // Disable code protection, disable background debugger, disable flash memory writing, disable EEPROM write protection, disable the Low-Voltage Programming, enable the Brown-Out Reset, enable the Power-On Timer, disable the watchdog timer, use a crystal oscillator

// Core frequency
#pragma CLOCK_FREQ 4000000

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
/** The alarm base address in RTC RAM. */
#define MAIN_ALARM_BASE_ADDRESS 0x08

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** The string corresponding to each day index. */
static unsigned char *String_Day_Names[] =
{
	NULL, // Index 0 does not exist
	"DIM",
	"LUN",
	"MAR",
	"MER",
	"JEU",
	"VEN",
	"SAM"
};	

//--------------------------------------------------------------------------------------------------
// Interrupts handler
//--------------------------------------------------------------------------------------------------
void interrupt(void)
{
	// Handle the ring interrupt
	if (RING_HAS_INTERRUPT_FIRED()) RingInterruptHandler();
	
	// Handle the snooze button interrupt
	if (BUTTON_HAS_INTERRUPT_FIRED())
	{
		RingStop(); // Stop the alarm in case it was ringing
		DisplayBacklightOn();
		
		// Clear the interrupt flag
		BUTTON_CLEAR_INTERRUPT_FLAG();
	}
	
	// Handle the display backlight timer
	if (DISPLAY_HAS_INTERRUPT_FIRED()) DisplayInterruptHandler();
	
	// Handle the serial port used to configure the clock
	if (UART_HAS_INTERRUPT_FIRED()) UARTInterruptHandler();
}

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
/** Convert a one-byte Binary Coded Decimal number to two ASCII characters.
 * @param BCD_Number The BCD number to convert.
 * @param Pointer_Tens_Character On output, contain the binary value of the number's tens.
 * @param Pointer_Units_Character On output, contain the binary value of the number's units.
 */
inline void MainConvertBCDToASCII(unsigned char BCD_Number, unsigned char *Pointer_Tens_Character, unsigned char *Pointer_Units_Character)
{
	*Pointer_Tens_Character = (BCD_Number >> 4) + '0';
	*Pointer_Units_Character = (BCD_Number & 0x0F) + '0';
}

//--------------------------------------------------------------------------------------------------
// Entry point
//--------------------------------------------------------------------------------------------------
void main(void)
{
	TRTCClockData Clock_Data;
	unsigned char i, Tens_Character, Units_Character, Temperature, Alarm_Hour, Alarm_Minutes;

	// Initialize the modules
	TemperatureSensorInitialize(); // Must be called before RTCInitialize() as TemperatureSensorInitialize() initializes the port A used by the RTC code too
	RTCInitialize();
	UARTInitialize();
	RingInitialize();
	DisplayInitialize();
	ButtonInitialize();
	
	// Enable interrupts
	intcon.PEIE = 1; // Enable peripherals interrupts
	intcon.GIE = 1; // Enable all interrupts
	
	// Load the alarm stored in the RTC RAM, so it can survive a power loss
	RTCSetReadAddress(MAIN_ALARM_BASE_ADDRESS);
	Alarm_Hour = RTCReadByte();
	Alarm_Minutes = RTCReadByte();
	
	while (1)
	{
		RTC_WAIT_TICK_BEGINNING();
		
		// Were new configuration data received from the UART ?
		if (UARTAreConfigurationDataAvailable(&Clock_Data, &Alarm_Hour, &Alarm_Minutes))
		{
			// Set the new RTC date and time
			RTCSetDateAndTime(&Clock_Data);
			
			// Save the alarm to the RTC RAM
			RTCWriteByte(MAIN_ALARM_BASE_ADDRESS, Alarm_Hour);
			RTCWriteByte(MAIN_ALARM_BASE_ADDRESS + 1, Alarm_Minutes);
		}
		
		// Get the date and time to display
		RTCGetDateAndTime(&Clock_Data);
		
		// Display hours
		MainConvertBCDToASCII(Clock_Data.Register_Name.Hours, &Tens_Character, &Units_Character);
		if (Tens_Character == '0') Tens_Character = ' '; // Do not display the leading zero
		DisplaySetCursorLocation(0);
		DisplayWriteCharacter(Tens_Character);
		DisplayWriteCharacter(Units_Character);
		DisplayWriteCharacter(':');
		
		// Display minutes
		MainConvertBCDToASCII(Clock_Data.Register_Name.Minutes, &Tens_Character, &Units_Character);
		DisplayWriteCharacter(Tens_Character);
		DisplayWriteCharacter(Units_Character);
		DisplayWriteCharacter(':');
		
		// Display seconds
		MainConvertBCDToASCII(Clock_Data.Register_Name.Seconds, &Tens_Character, &Units_Character);
		DisplayWriteCharacter(Tens_Character);
		DisplayWriteCharacter(Units_Character);
		
		// Display temperature
		// Sample the temperature
		Temperature = TemperatureSensorGetTemperature();
		// Convert the binary value to digits
		Tens_Character = Temperature / 10;
		Units_Character = Temperature - ((Tens_Character << 3) + (Tens_Character << 1)); // Units = Temperature - Integer_Division(Temperature, 10)
		// Display the value
		DisplaySetCursorLocation(0x0C);
		DisplayWriteCharacter(Tens_Character + '0');
		DisplayWriteCharacter(Units_Character + '0');
		DisplayWriteCharacter(0xDF); // An equivalent of the "degree" character with the japanese character map version
		DisplayWriteCharacter('C');
		
		// Display the day of the week
		DisplaySetCursorLocation(0x41); // Second line
		DisplayWriteCharacter(String_Day_Names[Clock_Data.Register_Name.Day_Of_Week][0]);
		DisplayWriteCharacter(String_Day_Names[Clock_Data.Register_Name.Day_Of_Week][1]);
		DisplayWriteCharacter(String_Day_Names[Clock_Data.Register_Name.Day_Of_Week][2]);
		DisplayWriteCharacter(' ');
		
		// Display the day
		MainConvertBCDToASCII(Clock_Data.Register_Name.Day, &Tens_Character, &Units_Character);
		DisplayWriteCharacter(Tens_Character);
		DisplayWriteCharacter(Units_Character);
		DisplayWriteCharacter('/');
		
		// Display the month
		MainConvertBCDToASCII(Clock_Data.Register_Name.Month, &Tens_Character, &Units_Character);
		DisplayWriteCharacter(Tens_Character);
		DisplayWriteCharacter(Units_Character);
		DisplayWriteCharacter('/');
		
		// Display the year
		MainConvertBCDToASCII(Clock_Data.Register_Name.Year, &Tens_Character, &Units_Character);
		DisplayWriteCharacter('2');
		DisplayWriteCharacter('0');
		DisplayWriteCharacter(Tens_Character);
		DisplayWriteCharacter(Units_Character);
		
		// Is it time to ring ?
		if (ButtonIsAlarmEnabled() && (Clock_Data.Register_Name.Hours == Alarm_Hour) && (Clock_Data.Register_Name.Minutes == Alarm_Minutes) && (Clock_Data.Register_Name.Seconds == 0x00)) RingStart();
		
		RTC_WAIT_TICK_END();
	}
}