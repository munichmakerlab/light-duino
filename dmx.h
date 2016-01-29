#ifndef dmx_h
#define dmx_h

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

#include <ESPDMX.h>               //https://github.com/Rickgg/ESP-Dmx
  
DMXESPSerial dmx;


// vars to hold state
int dmxChannels[intMaxChannel] = { 255 };


// rx tx switching logic

void prepare_send() {
  digitalWrite(rx_tx_switch_pin, HIGH);
}

void end_send() {
  digitalWrite(rx_tx_switch_pin, LOW);
}

void updateDMX() {
  prepare_send();
  delay(10);
  dmx.update();
  end_send();
}

// State communication

void updateStatesEntry() {
  String strStates;
  for( int i = 0; i < intMaxChannel; i++ ) {
    strStates = strStates + i + ":" + dmxChannels[i] + ",";
  } 
  strStates.remove(strStates.length() - 1);

  // set mqtt state msg with retain set to true
  publishMQTTMessage(strTopicPrefixID + "state", strStates, true);  
}

// Channel handling

void channelValue(int channel, int value) {
  dmx.write(channel, value);
  dmxChannels[channel] = value;
  DEBUG_PRINT("Switching ");
  DEBUG_PRINT(channel);
  DEBUG_PRINT(" to ");
  DEBUG_PRINTLN(value);
}

void channelOn(int channel) {
  channelValue(channel, 255);
  dmxChannels[channel] = 255;
}

void channelOff(int channel) {
  channelValue(channel, 0);
  dmxChannels[channel] = 0;
}

void toggleChannel(int channel) {
  if (dmxChannels[channel] > 0) {
    channelOff(channel);
  } else {
    channelOn(channel);
  } 
}

void dmxApplyChanges() {
  // Not necessary anymore, replaced by ticker interrupt
  //updateDMX();

  if (!dmxChangedStates) 
    triggedChange = millis();
    
  lastChange = millis();
    
  dmxChangedStates = 1;
  DEBUG_PRINTLN("Applied pending changes to DMX");
  
  updateStatesEntry();
}

void allChannelsOff() {
  DEBUG_PRINTLN("Switching all channels to off");
  for ( int i = 0; i < intMaxChannel; i++ ) {
    channelOff(i);
  } 
  dmxApplyChanges();
}


// setup functions

void setupDmx() {
  pinMode(rx_tx_switch_pin, OUTPUT);
  dmx.init(intMaxChannel);
  
}

#endif 
