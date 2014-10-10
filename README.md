#Teensy-AFX2
===========

This is a basic connection program for MIDI communcation between a Teensy 3.0 
with a MIDI I/O on TX/RX pins using MIDI 2.6 library.
Also uses Adafruit RGB LCD shield for display and buttons.

Some Notes:
- For adafruit RGB LCD to work with Teensy, need to change the lines with both
cpp files (LCD & MCP23017) from Wire1 to Wire, otherwise it won't compile
- This is not compatible with MIDI 3.0 library.
