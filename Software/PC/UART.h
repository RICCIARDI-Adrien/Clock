/** @file UART.h
 * Easy-to-use RS-232 communication layer.
 * @author Adrien RICCIARDI
 */
#ifndef H_UART_H
#define H_UART_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Initialize the PC UART at the specified baud rate, 8 data bits, no parity, 1 stop bit.
 * @param String_Device_File_Name The name of the UART device, like "/dev/ttyS0".
 * @param Baud_Rate The desired baud rate. The function may fail if this is not a standard value.
 * @return 0 if the UART was correctly initialized,
 * @return 1 if an error happened. See errno to find the error.
 */
int UARTOpen(char *String_Device_File_Name, unsigned int Baud_Rate);

/** Read a byte from the UART.
 * @return The read byte.
 * @warning This is a blocking function.
 */
unsigned char UARTReadByte(void);

/** Read several bytes from the UART.
 * @param Pointer_Buffer On output, contain the read bytes.
 * @param Bytes_Count How many bytes to read.
 * @note The function will block until all requested bytes are read.
 */
void UARTReadBuffer(void *Pointer_Buffer, unsigned int Bytes_Count);

/** Write a byte to the UART.
 * @param Byte The byte to send.
 */
void UARTWriteByte(unsigned char Byte);

/** Write several bytes to the UART.
 * @param Pointer_Buffer The data to write.
 * @param Bytes_Count How many bytes of data to write.
 */
void UARTWriteBuffer(void *Pointer_Buffer, unsigned int Bytes_Count);

/** Check if a byte was received by the UART.
 * @param Pointer_Available_Byte Store On output, contain the received byte if there was one available.
 * @return 0 if no byte was received (and Pointer_Available_Byte has unknown value),
 * @return 1 if a byte is available (in this case the byte is stored into Pointer_Available_Byte).
 */
int UARTIsByteAvailable(unsigned char *Pointer_Available_Byte);

/** Restore previous parameters and release the UART. */
void UARTClose(void);

#endif