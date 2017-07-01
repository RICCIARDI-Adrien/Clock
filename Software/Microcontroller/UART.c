/** @file UART.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "RTC.h"
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
/** The UART protocol magic number. */
#define UART_PROTOCOL_MAGIC_NUMBER 0xA5 // This value can't be represented in BCD format, so it can't be mistaken with data value

//--------------------------------------------------------------------------------------------------
// Private types
//--------------------------------------------------------------------------------------------------
/** All configuration protocol steps. */
typedef enum
{
	UART_PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER,
	UART_PROTOCOL_STATE_RECEIVE_SECONDS,
	UART_PROTOCOL_STATE_RECEIVE_MINUTES,
	UART_PROTOCOL_STATE_RECEIVE_HOURS,
	UART_PROTOCOL_STATE_RECEIVE_DAY_OF_WEEK,
	UART_PROTOCOL_STATE_RECEIVE_DAY,
	UART_PROTOCOL_STATE_RECEIVE_MONTH,
	UART_PROTOCOL_STATE_RECEIVE_YEAR,
	UART_PROTOCOL_STATE_RECEIVE_ALARM_HOUR,
	UART_PROTOCOL_STATE_RECEIVE_ALARM_MINUTES
} TUARTProtocolState;

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** Keep the latest received configuration clock data. */
static TRTCClockData UART_Configuration_RTC_Clock_Data;
/** Keep the lastest received alarm hour. */
static unsigned char UART_Configuration_Alarm_Hour;
/** Keep the lastest received alarm minutes. */
static unsigned char UART_Configuration_Alarm_Minutes;

/** Tell whether new configuration data have been received. */
static unsigned char UART_Are_Configuration_Data_Available = 0;

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
	
	// Enable UART reception interrupt
	pie1.RCIE = 1;
}

void UARTInterruptHandler(void)
{
	static TUARTProtocolState UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER;

	switch (UART_Protocol_State)
	{
		// Wait for the PC to send the magic number
		case UART_PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER:
			if (rcreg == UART_PROTOCOL_MAGIC_NUMBER)
			{
				// Send the acknowledge code
				txreg = UART_PROTOCOL_MAGIC_NUMBER; // There is no need for a specific state to wait for the byte to be sent because the PC will wait for this answer to sent the next byte
				
				UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_SECONDS;
			}
			break;
			
		// Receive seconds
		case UART_PROTOCOL_STATE_RECEIVE_SECONDS:
			UART_Configuration_RTC_Clock_Data.Register_Name.Seconds = rcreg;
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_MINUTES;
			break;
			
		// Receive minutes
		case UART_PROTOCOL_STATE_RECEIVE_MINUTES:
			UART_Configuration_RTC_Clock_Data.Register_Name.Minutes = rcreg;
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_HOURS;
			break;
			
		// Receive hours
		case UART_PROTOCOL_STATE_RECEIVE_HOURS:
			UART_Configuration_RTC_Clock_Data.Register_Name.Hours = rcreg;
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_DAY_OF_WEEK;
			break;
			
		// Receive day of week
		case UART_PROTOCOL_STATE_RECEIVE_DAY_OF_WEEK:
			UART_Configuration_RTC_Clock_Data.Register_Name.Day_Of_Week = rcreg;
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_DAY;
			break;
			
		// Receive day
		case UART_PROTOCOL_STATE_RECEIVE_DAY:
			UART_Configuration_RTC_Clock_Data.Register_Name.Day = rcreg;
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_MONTH;
			break;
		
		// Receive month
		case UART_PROTOCOL_STATE_RECEIVE_MONTH:
			UART_Configuration_RTC_Clock_Data.Register_Name.Month = rcreg;
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_YEAR;
			break;
			
		// Receive year
		case UART_PROTOCOL_STATE_RECEIVE_YEAR:
			UART_Configuration_RTC_Clock_Data.Register_Name.Year = rcreg;
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_ALARM_HOUR;
			break;
			
		// Receive alarm hour
		case UART_PROTOCOL_STATE_RECEIVE_ALARM_HOUR:
			UART_Configuration_Alarm_Hour = rcreg;
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_ALARM_MINUTES;
			break;
			
		// Receive alarm minutes
		case UART_PROTOCOL_STATE_RECEIVE_ALARM_MINUTES:
			UART_Configuration_Alarm_Minutes = rcreg;
			
			// Send an acknowledge to the PC telling that everything was successfully received
			txreg = UART_PROTOCOL_MAGIC_NUMBER;
			
			UART_Are_Configuration_Data_Available = 1;
			
			UART_Protocol_State = UART_PROTOCOL_STATE_RECEIVE_MAGIC_NUMBER;
			break;
	}
}

unsigned char UARTAreConfigurationDataAvailable(TRTCClockData *Pointer_Clock_Data, unsigned char *Pointer_Alarm_Hour, unsigned char *Pointer_Alarm_Minutes)
{
	unsigned char i;

	if (!UART_Are_Configuration_Data_Available) return 0;
	
	// Fill configuration data
	for (i = 0; i < sizeof(TRTCClockData); i++) Pointer_Clock_Data->Array[i] = UART_Configuration_RTC_Clock_Data.Array[i];
	*Pointer_Alarm_Hour = UART_Configuration_Alarm_Hour;
	*Pointer_Alarm_Minutes = UART_Configuration_Alarm_Minutes;
	
	// Configuration has been applied
	UART_Are_Configuration_Data_Available = 0;
	return 1;
}
