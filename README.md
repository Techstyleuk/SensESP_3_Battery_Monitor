# SensESP_3_Battery_Monitor
SensESP setup for monitoring 3 boat batteries

This is a work in progress, but so far works for the most part.

Hardware side
what I bought was:
DAOKI 5PCS INA219 GY-219 Bi-Direction DC Current Power Supply Breakout Sensor Module MCU-219 DIY - https://amzn.to/3H53e0V
I needed 3 of them so look carefully on the board you will see pads labelled A0 and A1 for battery A, I left the pads alone (default Address), for Battery B, I connected the A0 pad to the pad next to it (0x41) and for Battery C I connected A1 to the pad next to it (0x44)
