/** @file RTC.h
 * Access to the DS1307 Real-Time Clock through the I2C module.
 * @author Adrien RICCIARDI
 */
#ifndef H_RTC_H
#define H_RTC_H

//--------------------------------------------------------------------------------------------------
// Constants and macros
//--------------------------------------------------------------------------------------------------
/** Poll until a new RTC 1Hz tick begins. */
#define RTC_WAIT_TICK_BEGINNING() while (!portb.1)
/** Poll until the current RTC 1Hz tick ends. */
#define RTC_WAIT_TICK_END() while (portb.1)

//--------------------------------------------------------------------------------------------------
// Types
//--------------------------------------------------------------------------------------------------
/** All the RTC clock-related registers. They must be in the same order than the RTC register map to allow this struct to be accessed as an array of registers. */
typedef struct
{
	unsigned char Seconds; //! The seconds in range [0;59].
	unsigned char Minutes; //! The minutes in range [0;59].
	unsigned char Hours; //! The hours in range [0;23].
	unsigned char Day_Of_Week; //! The day of the week in range [0;7]. 0 stands for sunday, 1 for monday and so on.
	unsigned char Day; //! The day of the month in range [1;31].
	unsigned char Month; //! The month in range [1;12].
	unsigned char Year; //! The year in range [0;99].
} TRTCClockRegisters;

/** The RTC clock time and date. These data can be accessed as raw array or by register name. */
typedef union
{
	unsigned char Array[sizeof(TRTCClockRegisters)]; //! Raw access to the register map.
	TRTCClockRegisters Register_Name; //! Named access to the registers.
} TRTCClockData;

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the I2C module used to communicate with the RTC and configure the RTC to trigger an interrupt each second. */
void RTCInitialize(void);

/** Get the current date and time values.
 * @param Pointer_Clock_Data On output, will contain the current date and time in BCD format.
 */
void RTCGetDateAndTime(TRTCClockData *Pointer_Clock_Data);

/** Set the RTC date and time. Setting an invalid parameter will abort the RTC configuration and do nothing.
 * @param Pointer_Clock_Data The clock data to set, all parameters must be in BCD format.
 */
void RTCSetDateAndTime(TRTCClockData *Pointer_Clock_Data);

#endif