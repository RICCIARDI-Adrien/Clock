/** @file Button.h
 * Ease access to Alarm and Snooze buttons.
 * @author Adrien RICCIARDI
 */
#ifndef H_BUTTON_H
#define H_BUTTON_H

#include <system.h>

//--------------------------------------------------------------------------------------------------
// Constants and macros
//--------------------------------------------------------------------------------------------------
/** Tell if the snooze button interrupt fired or not. */
#define BUTTON_HAS_INTERRUPT_FIRED() intcon.INTF
/** Clear the snooze button interrupt flag. */
#define BUTTON_CLEAR_INTERRUPT_FLAG() intcon.INTF = 0

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the alarm and snooze buttons pins. */
void ButtonInitialize(void);

/** Test the alarm button to check whether the alarm is enabled or not
 * @return 0 if the alarm is disabled,
 * @return 1 if the alarm is enabled.
 */
#define ButtonIsAlarmEnabled() portc.2

#endif