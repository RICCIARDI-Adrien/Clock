/** @file Ring.c
 * @see Ring.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Button.h"
#include "Ring.h"

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
/** The timer reload value to get the requested 50Hz interrupt rate (called Finterrupt below). */
#define RING_TIMER_RELOAD_VALUE 60 // Reload_Value = 255 - ((Fosc/4) / (256 * Finterrupt))

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** The ring "melody". */
static unsigned char Ring_Tone[] =
{
	1, 1, 1,
	0, 0, 0,
	1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};

/** The ring "note" index. */
static unsigned char Ring_Tone_Index;

/** How many times the melody will be played before being automatically shut off. */
static unsigned char Ring_Melody_Loops_Count;

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void RingInitialize(void)
{
	// Configure the buzzer pin as output
	portc.1 = 0; // Avoid ringing the buzzer due to an undefined value in the port register
	trisc.1 = 0; // Use this pin as it can be configured as PWM if needed
	
	// Configure the timer 0 module to trigger an interrupt at 20Hz
	option_reg.T0CS = 0; // Use Fosc/4 as clock source
	option_reg.PSA = 0; // Assign the prescaler to the timer 0 module
	option_reg |= 0x07; // Set a 1:256 prescaler
}

void RingStart(void)
{
	// Preload the timer to get the requested interrupt rate
	tmr0 = RING_TIMER_RELOAD_VALUE;
	
	// Enable the timer interrupt
	intcon.T0IF = 0; // Clear the interrupt flag to avoid triggering a spurious interrupt
	intcon.T0IE = 1;
	
	// Start from the melody first note
	portc.1 = Ring_Tone[0];
	Ring_Tone_Index = 0;
	
	// Melody will be played this amount of time then will be shut down if the user did not 
	Ring_Melody_Loops_Count = 60;
}

void RingStop(void)
{
	// Stop ringing
	portc.1 = 0;
	
	// Disable the timer interrupt
	intcon.T0IE = 0;
}

void RingInterruptHandler(void)
{
	// Stop the alarm if the alarm button is switched to "disabled", or if the alarm rang too long
	if ((!ButtonIsAlarmEnabled()) || (Ring_Melody_Loops_Count == 0)) RingStop();
	else
	{
		// Reload the timer
		tmr0 = RING_TIMER_RELOAD_VALUE;
		
		// Play next tone
		Ring_Tone_Index++;
		if (Ring_Tone_Index >= sizeof(Ring_Tone))
		{
			Ring_Tone_Index = 0;
			Ring_Melody_Loops_Count--; // Melody is restarting from the begining
		}
		portc.1 = Ring_Tone[Ring_Tone_Index];
	}
	
	// Clear interrupt flag
	intcon.T0IF = 0;
}