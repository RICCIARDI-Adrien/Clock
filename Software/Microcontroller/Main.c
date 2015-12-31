/** @file Main.c
 * Clock main loop.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Link.h"
#include "RTC.h"

//--------------------------------------------------------------------------------------------------
// Microcontroller configuration
//--------------------------------------------------------------------------------------------------
// Microcontroller fuses
#pragma DATA _CONFIG, _CP_OFF & _DEBUG_OFF & _WRT_ENABLE_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _PWRTE_ON & _WDT_OFF & _XT_OSC // Disable code protection, disable background debugger, disable flash memory writing, disable EEPROM write protection, disable the Low-Voltage Programming, enable the Brown-Out Reset, enable the Power-On Timer, disable the watchdog timer, use a crystal oscillator

// Core frequency
#pragma CLOCK_FREQ 4000000

void UARTWriteByte(unsigned char Byte)
{
	// Wait until the Tx line is free
	while (!pir1.TXIF);
	// Send data
	txreg = Byte;
}

//--------------------------------------------------------------------------------------------------
// Entry point
//--------------------------------------------------------------------------------------------------
void main(void)
{
	TRTCClockData data;
	unsigned char reg;

	// Initialize the modules
	RTCInitialize();
	
	// Enable interrupts
	// TODO
	
	// Configure UART pins as inputs
	trisc.6 = 1;
	trisc.7 = 1;

	spbrg = 12; // 19200 bit/s baud rate
	txsta = 0x26; // 8-bit transmission, transmission enabled, high speed
	rcsta = 0x90; // Reception and serial port module enabled

	portb.7 = 0;
	portb.6 = 0;
	trisb = 0x3F;

	while (1)
	{
		RTC_WAIT_TICK_BEGINNING();
		
		portb.7 = !portb.7;
		
		RTCGetDateAndTime(&data);
		
		reg = data.Register_Name.Minutes;
		UARTWriteByte(((reg & 0x70) >> 4) + 48);
		UARTWriteByte((reg & 0x0F) + 48);
		UARTWriteByte(':');
		
		reg = data.Register_Name.Seconds;
		UARTWriteByte(((reg & 0x70) >> 4) + 48);
		UARTWriteByte((reg & 0x0F) + 48);
		UARTWriteByte('\r');
		UARTWriteByte('\n');
		
		RTC_WAIT_TICK_END();
	}
	
	while (1);
}