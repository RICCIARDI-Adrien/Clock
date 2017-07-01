/** @file UART.h
 * A simple polling UART driver.
 * @author Adrien RICCIARDI
 */
#ifndef H_UART_H
#define H_UART_H

//--------------------------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------------------------
/** Tell whether the UART reception interrupt fired or not. */
#define UART_HAS_INTERRUPT_FIRED() pir1.RCIF

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the UART at 19200 bauds, 8 data bits, no parity, 1 stop bit. */
void UARTInitialize(void);

/** Block until a byte is received from the UART.
 * @return The read byte.
 */
unsigned char UARTReadByte(void);

/** Write a byte to the UART.
 * @param Byte The byte to send.
 */
void UARTWriteByte(unsigned char Byte);

/** Tell if a byte has been received or not.
 * @return 1 if a byte is available to read,
 * @return 0 if no byte was received.
 */
unsigned char UARTIsByteReceived(void);

/** Handle the configuration protocol. */
void UARTInterruptHandler(void);

/** Tell whether new configuration data have been received through the UART.
 * @param Pointer_Clock_Data On output, filled with new clock date and time values.
 * @param Pointer_Alarm_Hour On output, filled with new alarm hour value.
 * @param Pointer_Alarm_Minutes On output, filled with new alarm minutes value.
 * @note This function must be called when the provided variables are not accessed.
 * @note All provided variables are modified ONLY if new data are available, they are left unmodified if there is no new configuration to apply. This way you can safely call the function with the target variables (no need for temporary ones).
 * @return 0 if no new configuration data were received,
 * @return 1 when new configuration data are available and have been copied to the provided variables.
 */
unsigned char UARTAreConfigurationDataAvailable(TRTCClockData *Pointer_Clock_Data, unsigned char *Pointer_Alarm_Hour, unsigned char *Pointer_Alarm_Minutes);

#endif
