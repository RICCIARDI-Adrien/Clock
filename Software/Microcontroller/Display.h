/** @file Display.h
 * Driver for DEM16216SYH-LY display in 4-bit mode.
 * @author Adrien RICCIARDI
 */
#ifndef H_DISPLAY_H
#define H_DISPLAY_H

#include <system.h>

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the display subsystem. */
void DisplayInitialize(void);

/** Light the display backlight. */
#define DisplayBacklightOn() portc.5 = 1 // Force this function to be "inlined"

/** Turn off the display backlight. */
#define DisplayBacklightOff() portc.5 = 0 // Force this function to be "inlined"

/** Write a character at the current cursor location.
 * @parameter Character The character to display.
 */
void DisplayWriteCharacter(unsigned char Character);

/** Set the cursor location.
 * @param Location The new cursor location. Line 1 locations are in range [0; 0x0F], line 2 locations are in range [0x40; 0x4F].
 */
void DisplaySetCursorLocation(unsigned char Location);

#endif