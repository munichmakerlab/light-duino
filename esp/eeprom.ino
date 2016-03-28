#include <EEPROM.h>
long last_save = 0;

void saveDMXState() {
  saveDMXState(false);
}

void saveDMXState(bool force) {
    if (force || (last_save < lastChange && millis() >= lastChange + minimumWaitBetweenSaves) ) {  
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
      Serial.print("Saved channel state, checksum: ");
      Serial.println(checksum);
      
      last_save = millis();
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
     for( int i = 0; i < intMaxChannel; i++ ) {
      dmxValue = EEPROM.read(i + 1);
      channelValue(i, dmxValue);
    }
    dmxChangedStates = 0;
    updateStatesEntry();
  } else {
    Serial.print("Checksum verification failed! Saved/Calculated: ");
    Serial.print(saved_checksum);
    Serial.print("/");
    Serial.println(checksum);
    
    allChannelsOff();
    saveDMXState(true);
  }
}


// setup functions

void setupEEPROM() {
  // init ESP EEPROM
  EEPROM.begin(intMaxChannel + 1); // +1 for checksum

  Serial.println("Recalling saved channel state.");
  recallState();
}
