#ifndef config_h
#define config_h

/*
 *  light-duino v2
 *  MQTT <-> DMX controller with hw switches, based on ESP8266 
 *  See attached Readme.md for details
 *  
 *  This is based on the work of Jorgen (aka Juergen Skrotzky, JorgenVikingGod@gmail.com), buy him a beer. ;-) 
 *  Rest if not noted otherwise by Peter Froehlich, tarwin@tarwin.de - Munich Maker Lab e.V. (January 2016)
 *  
 *  Published under Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *  You'll find a copy of the licence text in this repo. 
 */

/*
 * Debug Serial Output
 */

#define _debug 1

/*
 *  All importend stuff is here
 */

// if you are using more then one controller on MQTT you need unique IDs
#define strDeviceID 1

// maximum number of channels 
#define intMaxChannel 60

// maximum mqtt input size
#define intMaxInputSize 100

// Wifi - Use WifiManger for cool web admin interface if wifi client connection failes. 
// May not be secure in production environment
const bool useWifiManager  =  0;
const char *ssid           =  "-";
const char *password       =  "-";
const int wifiTimeout      =  20000;

// mqtt topic
String strTopic = "DMX";

// mqtt broker config
const char *mqtt_host      =  "iot.eclipse.org";
const int   mqtt_port      =  8883;   // default with TLS 8883, without 1883
const char *mqtt_user      =  "user";
const char *mqtt_pass      =  "pass";
const char *mqtt_client_id =  "DMX" + strDeviceID;

// Switches
const bool switchesEnabled = 1;
// indices match to each other
int switchPin[]           = {4, 12, 14, 13}; // tested values on ESP12e: 4, 12-14 
int matchingDmxChannels[] = {1, 2, 3, 4};
int matchingDmxChannelsDoubleClick[] = {16, 15, 14, 13};

// EEPROM save delay
unsigned long minimumWaitBetweenSaves = 60000; // in ms
unsigned long maximumWaitBetweenSaves = 600000;

/*
 * helper variables for connection (ip, mac)
 */
byte mac[6];                          // the MAC address of your Wifi shield
String strIPAddr;
String strMac;
String strTopicPrefix;
String strTopicPrefixID;

// init EEPROM vars
unsigned long triggedChange    = 0;
unsigned long lastChange       = 0;
bool dmxChangedStates          = 0;

#endif //config_h
