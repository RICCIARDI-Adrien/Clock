# Clock
A digital clock displaying time, date and current room temperature on a LCD display.

## Hardware
The clock is powered by a PIC16F876 microcontroller clocked at 4MHz. It reads time and date from a DS1307 Real-Time Clock.  
Those and the temperature sampled from a LM35DZ temperature sensor are displayed on a 2x16 LCD display.  
This clock also features an alarm mode which relies on a 85dB buzzer to make the more noise possible.  
The schematics can be found in the Hardware directory. They were drawn using Cadsoft Eagle 7.5.  
  
![Main board schematics](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Hardware/Main_Board.png)

## Software
The microcontroller firmware source code can be found in the Software/Microcontroller directory. The compiled firmware is provided by the Software/Microcontroller/Release/Clock.hex file. The source code is a Sourceboost 7.30 BoostC project.  
The PC program uses a serial port to talk to the clock. It can be compiled using gcc on Linux systems or Cygwin on Windows.

