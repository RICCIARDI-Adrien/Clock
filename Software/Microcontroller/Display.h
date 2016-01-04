/** @file Display.h
 * Driver for DEM16216SYH-LY display in 4-bit mode.
 * @author Adrien RICCIARDI
 */
#ifndef H_DISPLAY_H
#define H_DISPLAY_H

#include <system.h>

//--------------------------------------------------------------------------------------------------
// Constants and macros
//--------------------------------------------------------------------------------------------------
/** How many seconds the backlight will remain lighted. */
#define DISPLAY_BACKLIGHT_ON_DELAY 6

/** Tell whether the display timer interrupt fired or not. */
#define DISPLAY_HAS_INTERRUPT_FIRED() (pie1.TMR1IE && pir1.TMR1IF)

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the display subsystem. */
void DisplayInitialize(void);

/** Light the display backlight for DISPLAY_BACKLIGHT_ON_DELAY seconds. */
void DisplayBacklightOn(void);

/** Write a character at the current cursor location.
 * @parameter Character The character to display.
 */
void DisplayWriteCharacter(unsigned char Character);

/** Set the cursor location.
 * @param Location The new cursor location. Line 1 locations are in range [0; 0x0F], line 2 locations are in range [0x40; 0x4F].
 */
void DisplaySetCursorLocation(unsigned char Location);

/** Handle the backlight timer. */
void DisplayInterruptHandler(void);

#endif