We'll use a mechanical relay: this will allow you to switch much higher
voltages than a pi could handle.  Copy the code from one of your earlier
labs and repurpose it.

For the moment: [read this description](http://arduinoinfo.mywikis.net/wiki/ArduinoPower#Optically-Isolated_Relays).

Checkin:
  1. Use the relay to flash an LED.
  2. Measure how quickly the relay can turn on and off.  For this you will connect to an input
     pin of the pi: set the relay on, and count how many cycles til the pin goes on.  Do the
     same for off.
  3. Use the opto-coupler.
