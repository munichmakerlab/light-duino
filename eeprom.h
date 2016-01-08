#ifndef eeprom_h
#define eeprom_h

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


// EEPROM methods

void forceEEPROMWrite() {
  triggedChange = millis() - maximumWaitBetweenSaves;
  dmxChangedStates = 1;
}

void saveDMXState() {
  if (dmxChangedStates) {
    if (millis() >= triggedChange + maximumWaitBetweenSaves || millis() >= lastChange + minimumWaitBetweenSaves ) {  
      // calculate checksum 
      int checksum = 0; 
      for( int i = 0; i < intMaxChannel; i++ ) {
        checksum += dmxChannels[i];
      }
      checksum = checksum / intMaxChannel;
      EEPROM.write(0, checksum);
          
      for( int i = 0; i < intMaxChannel; i++ ) {
        EEPROM.write(i + 1, dmxChannels[i]);
      }
      EEPROM.commit();
      dmxChangedStates = 0;
      DEBUG_PRINT("Saved channel state, checksum: ");
      DEBUG_PRINTLN(checksum);
    }
  }
}

void recallState() {
  int saved_checksum = EEPROM.read(0);
  int checksum = 0;
  int dmxValue; 
  
  for( int i = 0; i < intMaxChannel; i++ ) {
    dmxValue = EEPROM.read(i + 1);
    channelValue(i, dmxValue);
    checksum += dmxValue;
  } 
  checksum = checksum / intMaxChannel;  // not perfect, better then nothing. 

  if (saved_checksum == checksum) {
    dmxApplyChanges();
    dmxChangedStates = 0;
  } else {
    DEBUG_PRINT("Checksum verification failed! Saved/Calculated: ");
    DEBUG_PRINT(saved_checksum);
    DEBUG_PRINT("/");
    DEBUG_PRINTLN(checksum);
    
    allChannelsOff();
    forceEEPROMWrite();
  }
}


// setup functions

void setupEEPROM() {
  // init ESP EEPROM
  EEPROM.begin(intMaxChannel + 1); // +1 for checksum

  DEBUG_PRINTLN("Recalling saved channel state:");
  recallState();
}

#endif 
