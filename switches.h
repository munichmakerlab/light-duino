#ifndef switches_h
#define switches_h

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

int arrayEntries = sizeof(matchingDmxChannels)/sizeof(int);

// DEBOUNCE VARS
int buttonState[sizeof(matchingDmxChannels)] = {HIGH};      // the current reading from the input pin
int lastButtonState[sizeof(matchingDmxChannels)] = {HIGH};  // the previous reading from the input pin
long lastDebounceTime[sizeof(matchingDmxChannels)] = {0};  // the last time the output pin was toggled

long debounceDelay = 50;

// Debounce by Severin Schols
void debounce(int index) {
  // read the state of the switch into a local variable:
  int reading = digitalRead(switchPin[index]);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[index]) {
    // reset the debouncing timer
    lastDebounceTime[index] = millis();
  }
 
  if ((millis() - lastDebounceTime[index]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState[index]) {
      buttonState[index] = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState[index] == LOW) {
        DEBUG_PRINT("Got action from button on pin ");
        DEBUG_PRINTLN(switchPin[index]);
        toggleChannel(matchingDmxChannels[index]);
        dmxApplyChanges();
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState[index] = reading;
}
   
void checkSwitches() {
  for ( int i = 0; i < arrayEntries; i++ ) {
    debounce(i);
  } 
}

void setupSwitches() {
  DEBUG_PRINT("Setting up ");
  DEBUG_PRINT(arrayEntries);
  DEBUG_PRINTLN(" switches");
  for ( int i = 0; i < arrayEntries; i++ ) {
    pinMode(switchPin[i], INPUT_PULLUP);
  } 
}

#endif 
