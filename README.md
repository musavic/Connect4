# Connect4
A C++ connect4 game
README
Name: Mustafa Mohamed
ID: 1492791

INCLUDED FILES:
connect4.cpp
Makefile
README

PROJECT DESCRIPTION & ASSUMPTION:
this is a connect 4 game between the cpu and a player.
The goal is to get 4 pieces in a row lined up vertically, horizontally or diagonally.


ACCESSOIES USED:
1 x Arduino Mega Board

WIRING INSTRUCTIONS:

from bottom to top on the right side (the side with the pins).
Board Pin <---> Arduino Pin
===========================
GND             GND
Vin             5V
CLK             52
MISO            50
MOSI            51
CS              10
D/C             9
Y+              A2 (analog pin)
X+              4  (digital pin)
Y-              5  (digital pin)
X-              A3 (analog pin)
CCS             6


HOW TO RUN THE CODE:
Attach the display and joystick as descripted above
Connect the usb of the arduino to the computer
Enter the file that contains connect3.cpp and run "make upload"
Then follow the INSTRUCTIONS on the screen
