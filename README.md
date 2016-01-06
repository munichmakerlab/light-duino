# light-duino
Our new and improved DMX light controller with mqtt control, based on an ESP8266. 

## Configuration
The conig.h file should contain everything to configure the system to your needs. 

## MQTT
The path is build up like this: 
<topic>/<id>/<endpoint>
e.g.: DMX/1/set

### MQTT endpoints
* controller: MAC and IP, comma separated
* state: list of current state of DMX ids with state, comma separeted
 * e.g.: "0:0,1:0,2:255,3:0,4:0,5:0,6:0,7:0,8:0,9:0,10:0,11:0"
* set: interface to set DMX channels. Accepts list of DMX id:state, comma separated. 
 * state is int between 0-255 (0 is off), or 1000 (toggle between on and off)   
 * e.g.: "10:0,2:0,8:255,4:255" 
 
## Switches
Switches can be connected to the Pins 4 and 12-15. 
The internal Pull-ups on these pins will be activated if the pins are configured as switch.  
 
## DMX
A DMX transciver like MAX485 needs to be connected to GPIO 2 (U1TXD) of the ESP

## Ext. libraries

* [WiFiManager](https://github.com/tzapu/WiFiManager)
* [PubSubClient](https://github.com/Imroy/pubsubclient)
* [ESPDMX](https://github.com/Rickgg/ESP-Dmx)

