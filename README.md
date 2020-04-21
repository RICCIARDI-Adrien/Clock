# Clock

A digital clock displaying time, date and current room temperature on a LCD display.

![Clock](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Clock.jpg)

## Hardware

The clock is powered by a PIC16F876 microcontroller clocked at 4MHz. It reads time and date from a DS1307 Real-Time Clock.  
Those and the temperature sampled from a LM35DZ temperature sensor are displayed on a 2x16 LCD display.  
This clock also features an alarm mode which relies on a 85dB buzzer to make the more noise possible.  
The schematics can be found in the Hardware directory. They were drawn using Cadsoft Eagle 7.5.  
  
![Main board schematics](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Main_Board_Schematics.png)

## Software

The microcontroller firmware source code can be found in the Software/Microcontroller directory. The source code is a Sourceboost 7.30 BoostC project.  
The PC program uses a serial port to talk to the clock. It can be compiled using gcc on Linux systems or Cygwin on Windows.  
An Android application (working from Android 4.0.3 to latest Android version) is also available. Plug an USB to serial cable to an USB host capable Android device and use the app to program the clock. The app was compiled with Android Studio 2.2.2.  
  
Microcontroller .hex file and Android .apk can be found in the [GitHub Release page](https://github.com/RICCIARDI-Adrien/Clock/releases).

## Photo gallery

### PCB

![PCB top](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/PCB_Top.jpg)
![PCB bottom](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/PCB_Bottom.jpg)

### Casing assembly

![Casing rear](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Casing_Rear.jpg)
![Assembly 1](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Assembly_1.jpg)
![Assembly 2](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Assembly_2.jpg)
![Assembly 3](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Assembly_3.jpg)
![Assembly 4](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Assembly_4.jpg)
![Assembly 5](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Assembly_5.jpg)

### Backlight testing

![Backlight testing](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Backlight_Testing.jpg)

### Side view

![Backlight testing](https://github.com/RICCIARDI-Adrien/Clock/blob/master/Resources/Side_View.jpg)
