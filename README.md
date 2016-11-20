# Clock
A digital clock displaying time, date and current room temperature on a LCD display.

## Hardware
The clock is powered by a PIC16F876 microcontroller clocked at 4MHz. It reads time and date from a DS1307 Real-Time Clock.  
Those and the temperature sampled from a LM35DZ temperature sensor are displayed on a 2x16 LCD display.  
This clock also features an alarm mode which relies on a 85dB buzzer to make the more noise possible.  
The schematics can be found in the Hardware directory. They were drawn using Cadsoft Eagle 7.5.  
  
![Main board schematics](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Hardware/Main_Board.png)

## Software
The microcontroller firmware source code can be found in the Software/Microcontroller directory. The source code is a Sourceboost 7.30 BoostC project.  
The PC program uses a serial port to talk to the clock. It can be compiled using gcc on Linux systems or Cygwin on Windows.  
An Android application (working from Android 4.0.3 to latest Android version) is also available. Plug an USB to serial cable to an USB host capable Android device and use the app to program the clock. The app was compiled with Android Studio 2.2.2.

## Releases
* Microcontroller firmware (Intel .hex file) : [Clock.hex](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Software/Microcontroller/Release/Clock.hex)
* Android application (.apk file) : [app-release.apk](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Software/Android/Clock/app-release.apk)