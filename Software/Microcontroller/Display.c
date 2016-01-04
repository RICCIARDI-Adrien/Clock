/** @file Display.c
 * @see Display.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Display.h"

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
/** The R/S signal. */
#define DISPLAY_SIGNAL_RS portb.2
/** The R/W signal. */
#define DISPLAY_SIGNAL_RW portb.1
/** The E signal. */
#define DISPLAY_SIGNAL_E portb.3

/** The timer reload value to get the requested 2Hz interrupt rate (called Finterrupt below). */
#define DISPLAY_TIMER_RELOAD_VALUE 3036 // Reload_Value = 65536 - (((Fosc/4) / Prescaler)) / Finterrupt) with Prescaler = 8

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** Count how many half-seconds the backlight is being lighted. */
static unsigned char Display_Backlight_Half_Seconds_Counter; // The timer counts at 2Hz

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
/** Write a byte of command or data to the display.
 * @param Byte The byte to write.
 * @param Is_Data Set to 1 of the byte is a data byte, set to 0 if the byte is a command.
 */
inline void DisplayWrite(unsigned char Byte, unsigned char Is_Data)
{
	if (Is_Data) DISPLAY_SIGNAL_RS = 1;
	else DISPLAY_SIGNAL_RS = 0;
	DISPLAY_SIGNAL_RW = 0;
	
	// Send the upper nibble
	portb &= 0x0F; // Clear bits 7 to 4
	DISPLAY_SIGNAL_E = 1;
	portb |= Byte & 0xF0;
	DISPLAY_SIGNAL_E = 0;
	
	// Send the lower nibble
	portb &= 0x0F; // Clear bits 7 to 4
	DISPLAY_SIGNAL_E = 1;
	portb |= (Byte << 4) & 0xF0;
	DISPLAY_SIGNAL_E = 0;
}

// This polling wait seems to hang the display module
#if 0
/** Wait until the display becomes ready for another operation. */
inline void DisplayWaitForOperationEnd(void)
{
	unsigned char Status;
	
	DISPLAY_SIGNAL_RS = 0;
	DISPLAY_SIGNAL_RW = 1;
	
	while (1)
	{
		// Read the upper nibble
		DISPLAY_SIGNAL_E = 1;
		Status = portb & 0xF0;
		DISPLAY_SIGNAL_E = 0;
		
		// Discard the lower nibble
		DISPLAY_SIGNAL_E = 1;
		asm nop;
		DISPLAY_SIGNAL_E = 0;
		
		// Check BF bit
		if (!(Status & 0x80)) return;
	}
}
#endif

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void DisplayInitialize(void)
{
	// Configure pins
	trisc.5 = 0; // Display backlight
	trisb &= 0x01; // Set RB7 to RB1 as output
	
	// Wait 40ms as requested and even more to be sure
	delay_ms(50);
	
	// Send the initial Function Set command which is not in two parts
	DISPLAY_SIGNAL_RS = 0;
	DISPLAY_SIGNAL_RW = 0;
	portb.7 = 0;
	portb.6 = 0;
	portb.5 = 1;
	portb.4 = 1;
	DISPLAY_SIGNAL_E = 1;
	DISPLAY_SIGNAL_E = 0;
	delay_ms(1); // Wait at least 37�s
	
	// Send Function Set command a second time
	DisplayWrite(0x2C, 0); // Set the display interface to 4 bits, enable use of both lines and select 5x8 font
	delay_ms(1); // Wait at least 37�s
	
	// Send Function Set command a third time
	DisplayWrite(0x2C, 0); // Set the display interface to 4 bits, enable use of both lines and select 5x8 font
	delay_ms(1); // Wait at least 37�s
	
	// Send Display ON/OFF command
	DisplayWrite(0x0C, 0); // Enable the display, disable the cursor and it's location displaying
	delay_ms(1); // Wait at least 37�s
	
	// Send Display Clear command
	DisplayWrite(0x01, 0);
	delay_ms(1); // Wait at least 37�s
	
	// Send Entry Mode Set command
	DisplayWrite(0x06, 0); // Set cursor moving direction to right, disable display shifting (i.e. scrolling)
	delay_ms(1); // Wait at least 37�s
	
	// Configure timer 1 to be used as the backlight timer
	t1con = 0x30; // Select a 1:8 prescaler, disable the built-in oscillator circuit, use Fosc/4 as clock source, do not enable the timer
}

void DisplayBacklightOn(void)
{
	// Turn the backlight on
	portc.5 = 1;
	
	// Preload the timer and start it
	tmr1h = DISPLAY_TIMER_RELOAD_VALUE >> 8;
	tmr1l = (unsigned char) DISPLAY_TIMER_RELOAD_VALUE;
	t1con.TMR1ON = 1;
	
	// Enable the timer interrupt
	pie1.TMR1IE = 1;
	
	// Start counting elapsed time
	Display_Backlight_Half_Seconds_Counter = 0;
}

void DisplayWriteCharacter(unsigned char Character)
{
	DisplayWrite(Character, 1);
	delay_us(64); // Wait at least 37�s
}

void DisplaySetCursorLocation(unsigned char Location)
{
	DisplayWrite(0x80 | Location, 0);
	delay_us(64); // Wait at least 37�s
}

void DisplayInterruptHandler(void)
{
	Display_Backlight_Half_Seconds_Counter++;
	if (Display_Backlight_Half_Seconds_Counter >= DISPLAY_BACKLIGHT_ON_DELAY * 2)
	{
		// Turn off the display backlight
		portc.5 = 0;
		
		// Stop the timer
		t1con.TMR1ON = 0;
		pie1.TMR1IE = 0; // Disable the timer interrupt
	}
	else
	{
		// Reload the timer value
		t1con.TMR1ON = 0; // Stop the timer to avoid a glitch when reloading its value
		tmr1h = DISPLAY_TIMER_RELOAD_VALUE >> 8;
		tmr1l = (unsigned char) DISPLAY_TIMER_RELOAD_VALUE;
		t1con.TMR1ON = 1; // Restart the timer
	}
	
	// Clear interrupt flag
	pir1.TMR1IF = 0;
}