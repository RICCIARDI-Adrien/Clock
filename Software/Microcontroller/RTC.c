/** @file RTC.c
 * @see RTC.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "RTC.h"

//--------------------------------------------------------------------------------------------------
// Private constants and macros
//--------------------------------------------------------------------------------------------------
/** The RTC I2C address ready to be sent on the bus (i.e. shifted by one on the left). The R/#W bit must be OR'ed if needed. */
#define RTC_I2C_ADDRESS 0xD0

/** The R/#W bit Read value in the I2C protocol. */
#define RTC_I2C_ADDRESS_READ_WRITE_BIT_READ 1
/** The R/#W bit Write value in the I2C protocol. */
#define RTC_I2C_ADDRESS_READ_WRITE_BIT_WRITE 0

/** Send a Start condition over the I2C bus. */
#define RTC_I2C_SEND_START() \
{ \
	pir1.SSPIF = 0; \
	sspcon2.SEN = 1; \
}

/** Send a Stop condition over the I2C bus. */
#define RTC_I2C_SEND_STOP() sspcon2.PEN = 1

/** Wait until the previous I2C operation has finished. */
#define RTC_I2C_WAIT_OPERATION_END() \
{ \
	while (!pir1.SSPIF); \
	pir1.SSPIF = 0; \
}

//--------------------------------------------------------------------------------------------------
// Private types
//--------------------------------------------------------------------------------------------------
/** All the RTC registers indexes. */
typedef enum
{
	RTC_REGISTER_SECONDS,
	RTC_REGISTER_MINUTES,
	RTC_REGISTER_HOURS,
	RTC_REGISTER_DAY,
	RTC_REGISTER_DATE,
	RTC_REGISTER_MONTH,
	RTC_REGISTER_YEAR,
	RTC_REGISTER_CONTROL
} TRTCRegister;

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
/** Read a 8-bit register from the RTC.
 * @param Register The register to read.
 * @param Value The value to set.
 */
inline void RTCWriteRegister(TRTCRegister Register, unsigned char Value)
{
	// Send an I2C START
	RTC_I2C_SEND_START();
	RTC_I2C_WAIT_OPERATION_END();
	
	// Send the RTC I2C address
	sspbuf = RTC_I2C_ADDRESS | RTC_I2C_ADDRESS_READ_WRITE_BIT_WRITE;
	RTC_I2C_WAIT_OPERATION_END();
	
	// Send register address
	sspbuf = Register;
	RTC_I2C_WAIT_OPERATION_END();
	
	// Send register value
	sspbuf = Value;
	RTC_I2C_WAIT_OPERATION_END();
	
	// Send an I2C STOP
	RTC_I2C_SEND_STOP();
	RTC_I2C_WAIT_OPERATION_END();
	
	// The minimum bus free time between a STOP and a START must be at least 4.7µs, but the microcontroller is so slow that there is no need to take that into account
}

// TODO remove if useless
/** Convert a BCD value to binary.
 * @param Binary_Coded_Decimal_Value The value to convert. First BCD digit is held in bits 7..4, second BCD digit stays in bits 3..0.
 * @return The binary value of the BCD number.
 */
/*inline unsigned char RTCConvertBinaryCodedDecimalToBinary(unsigned char Binary_Coded_Decimal_Value)
{
	unsigned char Tens;
	
	// Extract the tens value
	Tens = (Binary_Coded_Decimal_Value >> 4) & 0x0F;
	
	// Multiply it by ten using shifts
	Tens = (Tens << 8) + (Tens << 2);
	
	return Tens + (Binary_Coded_Decimal_Value & 0x0F);
}*/

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void RTCInitialize(void)
{
	TRTCClockData Clock_Data;
	
	// Set I2C pins as inputs
	trisc.3 = 1;
	trisc.4 = 1;
	
	// Set the pin connected to SWQ/OUT as input
	trisb.1 = 1;
	
	// Initialize the I2C module at 100KHz
	sspstat = 0x80; // Disable the slew rate control as requested for 100KHz speed mode, input levels conform to I2C
	sspcon2 = 0x00; // Reset communication flags
	sspadd = 9; // Baud_Rate = Fosc / (4 * (SSPADD + 1)) => SSPADD = (Fosc / (4 * Baud_Rate)) - 1
	sspcon = 0x28; // Enable I2C module in Master mode
	
	// On the first RTC boot, the Clock Halt bit will be set and will prevent the clock from running, so clear this bit if needed
	RTCGetDateAndTime(&Clock_Data);
	if (Clock_Data.Register_Name.Seconds & 0x80) RTCWriteRegister(RTC_REGISTER_SECONDS, 0); // No need to set a valid seconds count as the RTC time and date are not configured
	
	// Configure the RTC to generate an interrupt each second
	RTCWriteRegister(RTC_REGISTER_CONTROL, 0x90);
}

void RTCGetDateAndTime(TRTCClockData *Pointer_Clock_Data)
{
	unsigned char i;
	
	// Set the register index to the first register to read doing a fake write
	// Send an I2C START
	RTC_I2C_SEND_START();
	RTC_I2C_WAIT_OPERATION_END();
	
	// Send the RTC I2C address
	sspbuf = RTC_I2C_ADDRESS | RTC_I2C_ADDRESS_READ_WRITE_BIT_WRITE;
	RTC_I2C_WAIT_OPERATION_END();
	
	// Send the first register address
	sspbuf = RTC_REGISTER_SECONDS;
	RTC_I2C_WAIT_OPERATION_END();
	
	// Send an I2C STOP
	RTC_I2C_SEND_STOP();
	RTC_I2C_WAIT_OPERATION_END();
	
	// Read all relevant registers
	// Send an I2C START
	RTC_I2C_SEND_START();
	RTC_I2C_WAIT_OPERATION_END();
	
	// Send the RTC I2C address
	sspbuf = RTC_I2C_ADDRESS | RTC_I2C_ADDRESS_READ_WRITE_BIT_READ;
	RTC_I2C_WAIT_OPERATION_END();
	
	// Read all registers
	sspcon2.ACKDT = 0; // Send an I2C ACK when a value is read
	for (i = 0; i < sizeof(TRTCClockData); i++)
	{
		// Receive a byte from the device
		sspcon2.RCEN = 1;
		RTC_I2C_WAIT_OPERATION_END();
		Pointer_Clock_Data->Array[i] = sspbuf; // It is possible to store all field in the same way because bits CH and 12/24 are both 0)
	
		// Send an I2C ACK or NACK to the device
		if (i == sizeof(TRTCClockData) - 1) sspcon2.ACKDT = 1; // Send an NACK on the last read
		sspcon2.ACKEN = 1;
		RTC_I2C_WAIT_OPERATION_END();
	}
	
	// Send an I2C STOP
	RTC_I2C_SEND_STOP();
	RTC_I2C_WAIT_OPERATION_END();
}

void RTCSetDateAndTime(TRTCClockData *Pointer_Clock_Data)
{
	// Stop the clock to prevent it updating the time and date in the same time they are set
	RTCWriteRegister(RTC_REGISTER_SECONDS, 0x80);
	
	// Configure all registers
	RTCWriteRegister(RTC_REGISTER_MINUTES, Pointer_Clock_Data->Register_Name.Minutes);
	RTCWriteRegister(RTC_REGISTER_HOURS, Pointer_Clock_Data->Register_Name.Hours);
	RTCWriteRegister(RTC_REGISTER_DAY, Pointer_Clock_Data->Register_Name.Day_Of_Week);
	RTCWriteRegister(RTC_REGISTER_DATE, Pointer_Clock_Data->Register_Name.Day);
	RTCWriteRegister(RTC_REGISTER_MONTH, Pointer_Clock_Data->Register_Name.Month);
	RTCWriteRegister(RTC_REGISTER_YEAR, Pointer_Clock_Data->Register_Name.Year);
	
	// Set the SECONDS register and re-enable the clock in the same time
	RTCWriteRegister(RTC_REGISTER_SECONDS, Pointer_Clock_Data->Register_Name.Seconds);
}