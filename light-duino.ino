#include <DmxSimple.h>

#include <SPI.h>
#include <Ethernet.h>

byte switch_pin[] = {6,2,4,5};
byte dmx_channels[] = {1,2,3,4};

// DEBOUNCE VARS
int buttonState[] = {LOW,LOW,LOW,LOW};             // the current reading from the input pin
int lastButtonState[] = {LOW,LOW,LOW,LOW};   // the previous reading from the input pin
long lastDebounceTime[] = {0,0,0,0};  // the last time the output pin was toggled

long debounceDelay = 50;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0x1E, 0xED
};
//IPAddress ip(192, 168, 0, 180);
byte ip[] = { 192, 168, 0, 180 };

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

String readString = String();

// vars to hold state
int dmx[] = {0,0,0,0,0,0,0,0,0,0,0,0};


void toggle_channel(int channel) {
  if (dmx[channel] > 0) {
    channel_off(channel);
  } else {
    channel_on(channel);
  } 
}

void channel_on(int channel) {
  DmxSimple.write(channel, 255);
  dmx[channel] = 255;
  Serial.print("Switch On: ");
  Serial.println(channel);
}

void channel_off(int channel) {
  DmxSimple.write(channel, 0);
  dmx[channel] = 0;
  Serial.print("Switch Off: ");
  Serial.println(channel);
}

void debounce(int channel) {
  // read the state of the switch into a local variable:
  int reading = digitalRead(switch_pin[channel]);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[channel]) {
    // reset the debouncing timer
    lastDebounceTime[channel] = millis();
  }
 
  if ((millis() - lastDebounceTime[channel]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState[channel]) {
      buttonState[channel] = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState[channel] == HIGH) {
        toggle_channel(dmx_channels[channel]);
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState[channel] = reading;
}
  
  
void check_switches() {
  debounce(0);
  debounce(1);
  debounce(2);
  debounce(3);
}


void setup_ethernet() {
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void setup() {
  Serial.begin(9600);
  
  setup_ethernet();
    
  DmxSimple.usePin(3);
  DmxSimple.maxChannel(12);
  
  pinMode(switch_pin[0], INPUT); 
  pinMode(switch_pin[1], INPUT); 
  pinMode(switch_pin[2], INPUT); 
  pinMode(switch_pin[3], INPUT); 
  
}


void loop() { 
  check_switches();

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    int reply_type = 0;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (readString.length() < 100) {
          //store characters to string 
          readString += c; 
        } 

        if (c == '\n') {
          readString.trim();
          Serial.println(readString);
          if (readString.startsWith("GET /status.json")) {
            reply_type = 1;
          } else if (readString.startsWith("GET /set/on/")) {
            reply_type = 1;
            byte i = byte(readString.charAt(12)) - 49;
            if (i < 4)
              channel_on(dmx_channels[i]);
            Serial.println(i);
          } else if (readString.startsWith("GET /set/off/")) {
            reply_type = 1;
            byte i = byte(readString.charAt(13)) - 49;
            if (i < 4)
             channel_off(dmx_channels[i]);
          }
          
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK"); 
            client.println("Connection: close");  // the connection will be closed after completion of the response
            
            if (reply_type == 0) {
              client.println("Content-Type: text/html");
              client.println("Refresh: 5");  // refresh the page automatically every 5 sec
              client.println();
              client.println("<!DOCTYPE HTML>");
              client.println("<html>");
              
              // output the value of each analog input pin
              for (int channel = 0; channel < sizeof(dmx_channels); channel++) {
                
                client.print("switch ");
                client.print(channel);
                client.print(" is ");
                client.print(dmx[dmx_channels[channel]]);
                client.println("<br />");
              }
              client.println("</html>");
            } else if (reply_type = 1) {
              client.println("Content-Type: text/plain");
              client.println();
              client.print("{ \"status\": [");
              for (int channel = 0; channel < sizeof(dmx_channels); channel++) {
                if (channel > 0)
                  client.print(",");             
                client.print(dmx[dmx_channels[channel]]);
              }
              client.println("]}");
            }
            break;
          }

          currentLineIsBlank = true;
          readString = "";
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
