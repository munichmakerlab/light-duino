
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
  * ToDo: 
  * - external switch to set all lights to off
  * - make OTA work
  */
 
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>         //https://github.com/Imroy/pubsubclient
#include <ESPDMX.h>               //https://github.com/Rickgg/ESP-Dmx

// for WiFiManager:
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

#include <EEPROM.h>

#include "config.h"
#include "helpers.h"
#include "mqtt.h"
#include "dmx.h"
#include "eeprom.h"
#include "switches.h"


/*
 * MQTT methods
 */

// callback method to handle received mqtt messages
void mqttMessageReceived(const MQTT::Publish& pub) {
  // handle message arrived
  mqttTopic = pub.topic();
  // OTA: "<topic>/<id>/ota", bin file to flash
  if (mqttTopic == String(strTopicPrefixID + "ota")) {

    //very cool, but kinda dangerous?? disabled for now...
    return;
    
    uint32_t startTime = millis();
    uint32_t size = pub.payload_len();
    if (size == 0)
      return;
    DEBUG_PRINT("Receiving OTA of ");
    DEBUG_PRINT(size);
    DEBUG_PRINTLN(" bytes...");
    Serial.setDebugOutput(true);
    if (ESP.updateSketch(*pub.payload_stream(), size, true, false)) {
      pub.payload_stream()->stop();
      DEBUG_PRINTLN("Clearing retained message.");
      mqttClient.publish(MQTT::Publish(pub.topic(), "").set_retain());
      mqttClient.disconnect();
      Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
      ESP.restart();
      delay(10000);    
    }
  } else {
    mqttPayload = pub.payload_string() + "0"; // trailing 0 needed for wonky but fast field separation later. 
    mqttNewMessage = true;
    DEBUG_PRINTLN("receive MQTT: topic='" + mqttTopic + "', message='" + mqttPayload + "'");
  }
}


bool initializeMQTT() {
  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_PRINTLN_NOSPAM("Initializing MQTT connection");
    if (!mqttClient.connected()) {
      if (connectMQTT(mqtt_user, mqtt_pass, mqtt_host)) {
        updateDMX();
        publishMQTTMessage(strTopicPrefixID + "controller", strMac + "," + strIPAddr, true);
        // bind callback function to handle incoming mqtt messages
        mqttClient.set_callback(mqttMessageReceived);
        // define what to listen to
        subscribeMQTTTopic(strTopicPrefixID + "set");

        return true;
      }
      return false;
    } else {
      return true;
    }
  }
  
  DEBUG_PRINTLN_NOSPAM("Waiting for WiFi...");
  return false;
}


void configModeCallback () {
  digitalWrite(BUILTIN_LED, LOW);  
}

void processMQTTMessage() {
  // reset flag
  mqttNewMessage = false;
  
  // DMX 'set' Topic: "DMX/<device_id>/set" 
  // payload syntax: multiple comma-sep-fields consisting of <dmx_id>:<brightness value (0-255) or 1000 to switch between on/dimmed and off>
  // e.g.: 1:0,2:128,3:255,4:x  
  if (mqttTopic == String(strTopicPrefixID + "set")) {
    char input[mqttPayload.length()]; // +1 neccessary for \0??
    mqttPayload.toCharArray(input, mqttPayload.length());
    
    // Read each command pair 
    char* command = strtok(input, ",");
    while (command != 0) {
        // Split the command in two values
        char* separator = strchr(command, ':');
        if (separator != 0) {
            // Actually split the string in 2: replace ':' with 0
            *separator = 0;
            int dmxID = atoi(command);
            if (dmxID <= intMaxChannel) {
              ++separator;
            
              int dmxValue = atoi(separator);
              // 255 is the maximum DMX understands. higher values are errors or special triggers
              if (dmxValue == 1000) {
                toggleChannel(dmxID);
                continue; 
              } 
              
              if (dmxValue > 255) {
                dmxValue = 255; 
              }
              //prepare dmx output: 
              channelValue(dmxID, dmxValue);
            } 
        }
        // Find the next command in input string
        command = strtok(0, ",");
    }
    //apply changes to dmx
    dmxApplyChanges();
  }
}

void setupWifi() {
  if (useWifiManager) {
    //WiFiManager
    
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset settings - for testing
    //wifiManager.resetSettings();
    //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);
    //fetches ssid and pass and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    wifiManager.setAPConfig(IPAddress(10,0,0,1), IPAddress(10,0,0,1), IPAddress(255,255,255,0));
    //and goes into a blocking loop awaiting configuration
    String strAPName = String(mqtt_client_id) + String("-") + String(ESP.getChipId());
    if(!wifiManager.autoConnect(strAPName.c_str())) {
      DEBUG_PRINTLN("failed to connect and hit timeout");
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(1000);
    }
  } else {
    long startConnectionAttempt = millis();

    DEBUG_PRINTLN("Starting WiFi");
    // use static wifi config
    WiFi.begin(ssid, password);

    // Wait for connection
    while(WiFi.status() != WL_CONNECTED && millis() - startConnectionAttempt > wifiTimeout) {
      delay(100);
      DEBUG_PRINT(".");
    }
  }  
   
  DEBUG_PRINTLN("WiFi connected!");
  DEBUG_PRINTLN("IP address: ");
  
  IPAddress local = WiFi.localIP();
  strIPAddr = String(local[0]) + "." + String(local[1]) + "." + String(local[2]) + "." + String(local[3]);
  DEBUG_PRINTLN(strIPAddr);
  
}


/*
 * ==================
 *      S E T U P
 * ==================
 */

void setup() {
  //debug Led on ESP
  pinMode(BUILTIN_LED, OUTPUT); 
  digitalWrite(BUILTIN_LED, HIGH);  // means led is off.
  
  // initial serial port
  if(_debug)
    Serial.begin(9600);
  DEBUG_PRINTLN("");
  DEBUG_PRINT("MQTT <-> DMX (");
  DEBUG_PRINT(ESP.getChipId());
  DEBUG_PRINTLN(")");
  DEBUG_PRINTLN(__TIMESTAMP__);
  DEBUG_PRINT("Sketch size: ");
  DEBUG_PRINTLN(ESP.getSketchSize());
  DEBUG_PRINT("Free size: ");
  DEBUG_PRINTLN(ESP.getFreeSketchSpace());

  // Mac address
  WiFi.macAddress(mac);
  byteToHexString(strMac, mac, 6, ":");
  DEBUG_PRINTLN("MAC: ");
  DEBUG_PRINTLN(strMac);
  
  // define topics
  strTopicPrefix = strTopic + "/";
  strTopicPrefixID = strTopicPrefix + strDeviceID + "/";

  // init DMX  
  setupDmx();

  // init EEPROM and set initial state
  setupEEPROM();

  // setup switches
  setupSwitches();

  // setup WiFi
  setupWifi(); 
  
  // check for connection
  initializeMQTT();
}


/*
 * ==================
 *      L O O P
 * ==================
 */

void loop() {    
  // check for connection and process MQTT
  if (mqttClient.loop()) {
    // MQTT connection is alive
    // process new mqtt messages
    if (mqttNewMessage)
      processMQTTMessage();
  } else {
    // No MQTT Connection, reinitialize
    initializeMQTT();
  }

  // check switches
  if (switchesEnabled)
    checkSwitches();

  // set current dmx channels on bus  
  updateDMX();

  // save dmx state 
  saveDMXState();
}

