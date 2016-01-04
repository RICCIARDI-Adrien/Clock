/** @file Button.c
 * @see Button.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Button.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void ButtonInitialize(void)
{
	// Set button pins as input
	trisb.0 = 1;
	trisc.2 = 1;
	
	// Enable the RB0 interrupt
	option_reg.INTEDG = 1; // Trigger the interrupt on a rising edge
	intcon.INTE = 1; // Enable the interrupt
}