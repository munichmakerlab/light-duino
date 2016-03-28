
#include "config.h"

void setupSwitches();
void checkSwitches();

int dmxChannels[intMaxChannel] = { 0 };

void toggleChannel(int channel);
void channelValue(int channel, int value);

// init EEPROM vars
unsigned long triggedChange    = 0;
unsigned long lastChange       = 0;
bool dmxChangedStates          = 0;

void updateStatesEntry();

bool dmxInHandler(char* input, unsigned int length) {
  
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

            Serial.print("DMX in: Channel ");
            Serial.print(dmxID);
            Serial.print(" Value ");
            Serial.println(dmxValue);
            
            // 255 is the maximum DMX understands. higher values are errors or special triggers
            
            if (dmxValue == 1000) {
              toggleChannel(dmxID); 
            } else {
              if (dmxValue > 255) {
                dmxValue = 255; 
              }
              //prepare dmx output: 
              channelValue(dmxID, dmxValue);
            }
          } 
      }
      // Find the next command in input string
      command = strtok(0, ",");
  }
  updateStatesEntry();
}

void setup() {
  // Default debug serial connection
  Serial.begin(115200);

  // Serial connection to the ATmega DMX sender
  Serial1.begin(9600);
  
  setup_mqtt();
  setupSwitches();
  setupEEPROM();
}

void loop() {
  loop_mqtt();
  if (switchesEnabled)
    checkSwitches();
  saveDMXState();
}
