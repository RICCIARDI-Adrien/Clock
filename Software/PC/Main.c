/** @file Main.c
 * Allow to configure the time, date and alarm of the digital clock.
 * @author Adrien RICCIARDI
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "UART.h"

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The UART protocol magic number. */
#define MAIN_UART_PROTOCOL_MAGIC_NUMBER 0xA5

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Close the serial port on program termination. */
static void MainExitCloseSerialPort(void)
{
	UARTClose();
}

/** Convert a 2-digit binary number to Binary Coded Decimal.
 * @param Number The number to convert.
 * @return the corresponding BCD value.
 */
static unsigned char MainConvertBinaryNumberToBCD(int Number)
{
	int Tens, Units;
	
	// Make sure the number can be represented on 1-byte BCD
	if ((Number < 0) || (Number > 99))
	{
		printf("[%s] Error : the number %d can't be converted to 1-byte BCD.\n", __func__, Number);
		exit(EXIT_FAILURE);
	}
	
	Tens = Number / 10;
	Units = Number - (Tens * 10);
	
	return (unsigned char) ((Tens << 4) | Units);
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	char *String_Serial_Port;
	int Result, Alarm_Hour, Alarm_Minutes;
	time_t Time;
	struct tm *Pointer_Converted_Time;
	
	// Check parameters
	if (argc != 4)
	{
		printf("Error : bad arguments.\n"
			"Usage : %s Serial_Port Alarm_Hour Alarm_Minutes\n"
			"Example : %s /dev/ttyUSB0 7 30\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	
	// Extract parameters
	// Serial port
	String_Serial_Port = argv[1];
	// Alarm hour
	Result = sscanf(argv[2], "%d", &Alarm_Hour);
	if ((Result != 1) || (Alarm_Hour < 0) || (Alarm_Hour > 23))
	{
		printf("Error : the alarm hour must be in range [0;23].\n");
		return EXIT_FAILURE;
	}
	// Alarm minutes
	Result = sscanf(argv[3], "%d", &Alarm_Minutes);
	if ((Result != 1) || (Alarm_Minutes < 0) || (Alarm_Minutes > 59))
	{
		printf("Error : the alarm minutes must be in range [0;59].\n");
		return EXIT_FAILURE;
	}
	
	// Try to open the serial port
	if (UARTOpen(String_Serial_Port, 19200) != 0)
	{
		printf("Error : failed to open the serial port '%s'.\n", String_Serial_Port);
		return EXIT_FAILURE;
	}
	atexit(MainExitCloseSerialPort);
	
	// Connect to the clock
	// Send the magic number to tell the clock that data will be sent
	printf("Connecting to the clock...");
	fflush(stdout);
	UARTWriteByte(MAIN_UART_PROTOCOL_MAGIC_NUMBER);
	// Wait for the answer
	while (UARTReadByte() != MAIN_UART_PROTOCOL_MAGIC_NUMBER);
	printf(" connected.\n");
	
	// Get the current date and time now that all blocking operations are done (for a better accuracy)
	Time = time(NULL);
	Pointer_Converted_Time = localtime(&Time);
	
	printf("Sending data...");
	fflush(stdout);
	// Send seconds
	UARTWriteByte(MainConvertBinaryNumberToBCD(Pointer_Converted_Time->tm_sec));
	// Send minutes
	UARTWriteByte(MainConvertBinaryNumberToBCD(Pointer_Converted_Time->tm_min));
	// Send hours
	UARTWriteByte(MainConvertBinaryNumberToBCD(Pointer_Converted_Time->tm_hour));
	// Send the day of the week (starting from 1 on the RTC)
	UARTWriteByte(MainConvertBinaryNumberToBCD(Pointer_Converted_Time->tm_wday + 1));
	// Send the day of the month
	UARTWriteByte(MainConvertBinaryNumberToBCD(Pointer_Converted_Time->tm_mday));
	// Send the month (starting from 1 on the RTC)
	UARTWriteByte(MainConvertBinaryNumberToBCD(Pointer_Converted_Time->tm_mon + 1));
	// Send the year (starting from 2000 on the RTC)
	UARTWriteByte(MainConvertBinaryNumberToBCD(Pointer_Converted_Time->tm_year - 100));
	// Send alarm hour
	UARTWriteByte(MainConvertBinaryNumberToBCD(Alarm_Hour));
	// Send alarm minutes
	UARTWriteByte(MainConvertBinaryNumberToBCD(Alarm_Minutes));
	
	// Wait for the clock answer
	while (UARTReadByte() != MAIN_UART_PROTOCOL_MAGIC_NUMBER);
	printf(" done.\nThe clock is successfully configured. You can unplug the cable.\n");
	
	return EXIT_SUCCESS;
}
