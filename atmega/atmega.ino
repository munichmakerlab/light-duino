#include <DmxSimple.h>

String data;

int intMaxChannel = 128;
long lastSerialEvent;
#define LINE_TIMEOUT 1000
int channel = 0;
int value = 0;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  delay(200);
  digitalWrite(13,LOW);
  delay(200);
  digitalWrite(13,HIGH);
  delay(200);
  digitalWrite(13,LOW);
  delay(200);
  digitalWrite(13,HIGH);
  delay(200);
  digitalWrite(13,LOW);

  DmxSimple.usePin(2);
  DmxSimple.maxChannel(4);
  data = "";
}

void loop() {
  if (millis() - lastSerialEvent > LINE_TIMEOUT) {
    resetChannel();
  }
  
  while (Serial.available()) {
    lastSerialEvent = millis();

    char inChar = Serial.read();

    if (inChar == ':') {
      channel = data.toInt();
      data = "";
    } else if (inChar == '\n' || inChar == ',' || inChar == ';') {
      value = data.toInt();
      DmxSimple.write(channel, value);
      resetChannel();
    } else {
      data += inChar;
    }
  }
}

void resetChannel() {
  data = "";
  channel = 0;
  value = 0;
}

