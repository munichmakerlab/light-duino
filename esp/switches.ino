int arrayEntries = sizeof(matchingDmxChannels)/sizeof(int);

// DEBOUNCE VARS
int buttonState[sizeof(matchingDmxChannels)] = {HIGH};      // the current reading from the input pin
int lastButtonState[sizeof(matchingDmxChannels)] = {HIGH};  // the previous reading from the input pin
bool activated[sizeof(matchingDmxChannels)] = {0};     // was the switch activated?
long lastActivationTime[sizeof(matchingDmxChannels)] = {0};     // record when the last action was triggered
long lastDebounceTime[sizeof(matchingDmxChannels)] = {0};   // the last time the output pin was toggled

long successionClickTimeout = 250;
long debounceDelay = 50;

void activateChannel(int index, int channel) {
  activated[index] = 0;
  toggleChannel(channel);
  updateStatesEntry();
}

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
  
    // if the activation flag is set, and we are over the double click timeout, trigger the action
    if (activated[index] && (millis() - lastActivationTime[index]) > successionClickTimeout) {
      activateChannel(index, matchingDmxChannels[index]);
    }

    // if the button state has changed:
    if (reading != buttonState[index]) {
      buttonState[index] = reading;

      // only toggle the activation if the new button state is HIGH
      if (buttonState[index] == LOW) {

        // if the activation flag is already set, trigger the double click action
        if (activated[index]) { 
          activateChannel(index, matchingDmxChannelsDoubleClick[index]);
        } else {
          // if not, just activate the change
          lastActivationTime[index] = millis();
          activated[index] = 1;
        }
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
  for ( int i = 0; i < arrayEntries; i++ ) {
    pinMode(switchPin[i], INPUT_PULLUP);
  } 
}
