# aerie digital thermostat

A simple Arduino based digital thermostat to control my boiler.

This is a prototype only at this point, utilizing:
* Arduino UNO
* DHT11 or DHT22 Temperature / Humidity sensor
* VMA203 Display / keypad module

Thermostat controls a relay to cycle boiler on / off.

Current features:
* User defined temperature set point
* Set temperature, actual temperature, humidity, and boiler state display
* Dimmable LCD display
* Minimum boiler cycle time protection (defined at compile time)
