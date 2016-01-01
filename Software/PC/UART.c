/** @file UART_Linux.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "UART.h"

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The serial port descriptor. */
static int File_Descriptor_UART;

/** Keep the previous UART parameters. */
static struct termios Parameters_Old;

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int UARTOpen(char *String_Device_File_Name, unsigned int Baud_Rate)
{
	struct termios Parameters_New;
	speed_t Speed;
	
	// Choose the right speed constant
	switch (Baud_Rate)
	{
		case 0:
			Speed = B0;
			break;
			
		case 50:
			Speed = B50;
			break;
			
		case 75:
			Speed = B75;
			break;
			
		case 110:
			Speed = B110;
			break;
			
		case 134:
			Speed = B134;
			break;
			
		case 150:
			Speed = B150;
			break;
			
		case 200:
			Speed = B200;
			break;
			
		case 300:
			Speed = B300;
			break;
		
		case 600:
			Speed = B600;
			break;
		
		case 1200:
			Speed = B1200;
			break;
			
		case 1800:
			Speed = B1800;
			break;
			
		case 2400:
			Speed = B2400;
			break;
			
		case 4800:
			Speed = B4800;
			break;
			
		case 9600:
			Speed = B9600;
			break;
			
		case 19200:
			Speed = B19200;
			break;
			
		case 38400:
			Speed = B38400;
			break;
			
		case 57600:
			Speed = B57600;
			break;
			
		case 115200:
			Speed = B115200;
			break;
			
		case 230400:
			Speed = B230400;
			break;
			
		default:
			return 1;
	}
	
	// Open the UART device file
	File_Descriptor_UART = open(String_Device_File_Name, O_RDWR | O_NONBLOCK);
	if (File_Descriptor_UART == -1) return 1;
	
	// Backup old UART parameters
	if (tcgetattr(File_Descriptor_UART, &Parameters_Old) == -1) goto Exit_Error;
	
	// Configure new parameters
	Parameters_New.c_iflag = IGNBRK | IGNPAR; // Ignore break, no parity
	Parameters_New.c_oflag = 0;
	Parameters_New.c_cflag = CS8 | CREAD | CLOCAL; // 8 data bits, receiver enabled, ignore modem control lines
	Parameters_New.c_lflag = 0; // Use raw mode
	
	// Set speeds
	if (cfsetispeed(&Parameters_New, Speed) == -1) goto Exit_Error;
	if (cfsetospeed(&Parameters_New, Speed) == -1) goto Exit_Error;
	
	// Set parameters
	if (tcsetattr(File_Descriptor_UART, TCSANOW, &Parameters_New) == -1) goto Exit_Error;
	return 0;
	
Exit_Error:
	close(File_Descriptor_UART);
	return 1;
}

unsigned char UARTReadByte(void)
{
	unsigned char Byte;
	
	while (read(File_Descriptor_UART, &Byte, 1) <= 0);
	return Byte;
}

void UARTReadBuffer(void *Pointer_Buffer, unsigned int Bytes_Count)
{
	unsigned char Byte, *Pointer_Buffer_Byte = Pointer_Buffer;
	
	while (Bytes_Count > 0)
	{
		// Try to get a byte
		if (read(File_Descriptor_UART, &Byte, 1) == 1)
		{
			*Pointer_Buffer_Byte = Byte;
			Pointer_Buffer_Byte++;
			Bytes_Count--;
		}
	}
}

void UARTWriteByte(unsigned char Byte)
{
	write(File_Descriptor_UART, &Byte, 1);
}

void UARTWriteBuffer(void *Pointer_Buffer, unsigned int Bytes_Count)
{
	write(File_Descriptor_UART, Pointer_Buffer, Bytes_Count);
}

int UARTIsByteAvailable(unsigned char *Available_Byte)
{
	if (read(File_Descriptor_UART, Available_Byte, 1) == 1) return 1;
	return 0;
}

void UARTClose(void)
{
	tcsetattr(File_Descriptor_UART, TCSANOW, &Parameters_Old);
	close(File_Descriptor_UART);
}	