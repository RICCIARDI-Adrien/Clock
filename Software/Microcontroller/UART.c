/** @file UART.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void UARTInitialize(void)
{
	// Configure UART pins as inputs
	trisc.6 = 1;
	trisc.7 = 1;

	// Configure the UART module
	spbrg = 12; // 19200 bit/s baud rate
	txsta = 0x26; // Select 8-bit transmission, enable transmission, use asynchronous mode, select high baud rate mode
	rcsta = 0x90; // Enable the serial port module and the reception
}

unsigned char UARTReadByte(void)
{
	// Wait for a byte to be received
	while (!pir1.RCIF);
	
	return rcreg;
}

void UARTWriteByte(unsigned char Byte)
{
	// Wait until the Tx line is free
	while (!pir1.TXIF);
	// Send data
	txreg = Byte;
}

unsigned char UARTIsByteReceived(void)
{
	if (pir1.RCIF) return 1;
	return 0;
}