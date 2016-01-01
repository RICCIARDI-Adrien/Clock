/** @file Ring.h
 * Make the buzzer ring and stop.
 * @author Adrien RICCIARDI
 */
#ifndef H_RING_H
#define H_RING_H

#include <system.h>

//--------------------------------------------------------------------------------------------------
// Constants and macros
//--------------------------------------------------------------------------------------------------
/** Tell if the ring interrupt fired or not. */
#define RING_HAS_INTERRUPT_FIRED() (intcon.T0IE && intcon.T0IF)

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the timer used to make the buzzer ring. */
void RingInitialize(void);

/** Make the buzzer ring. */
void RingStart(void);

/** Stop ringing. */
void RingStop(void);

/** Must be called everytime the timer 0 interrupt fires. */
void RingInterruptHandler(void);

#endif