/** @file UART.h
 * A simple polling UART driver.
 * @author Adrien RICCIARDI
 */
#ifndef H_UART_H
#define H_UART_H

//--------------------------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------------------------
/** The UART protocol magic number. */
#define UART_PROTOCOL_MAGIC_NUMBER 0xA5 // This value can't be represented in BCD format, so it can't be mistaken with data value

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

#endif