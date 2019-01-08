# arduinoscript
Port of Total Spectrum's tinyscript to the Arduino platform

## Using it
Copy the repo to your libraries folder, the  try out the REPL sketch. It's a pretty basic C like language. 
See https://github.com/totalspectrum/tinyscript for documentation.

This is all experimental and subject to change.

In this fork, numbers are 64 bit signed ints. You'll get error -6 if a command tries to run for more that 250k
instructions.

## Available Functions and Constants

### pinMode
### digitalWrite
### digitalRead
### millis
### delay

### HIGH
### LOW
### INPUT
### OUTPUT
### INPUT_PULLUP

All of those directly mirror the standard Arduino equivalents