// Main DMX update routine
void channelValue(int channel, int value) {
  // Send DMX channel and value to ATmega DMX sender
  Serial1.print(channel);
  Serial1.print(":");
  Serial1.println(value);

  // Store channel in lookup array
  dmxChannels[channel] = value;

  // Update timestamp
  lastChange = millis();
}

// Wrapper: Set channel to 255
void channelOn(int channel) {
  channelValue(channel, 255);
}

// Wrapper: Set channel to 0
void channelOff(int channel) {
  channelValue(channel, 0);
}

// Wrapper: toggle channel between on and off
void toggleChannel(int channel) {
  if (dmxChannels[channel] > 0) {
    channelOff(channel);
  } else {
    channelOn(channel);
  } 
}

// Wrapper: turn all channels off
void allChannelsOff() {
  Serial.println("Switching all channels to off");
  for ( int i = 0; i < intMaxChannel; i++ ) {
    channelOff(i);
  }
}
