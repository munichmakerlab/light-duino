
// This defines named DMX channels 
#define LIGHT1   1
#define LIGHT2   2
#define LIGHT3   3
#define LIGHT4   4

// defines buttons to switch lights
#define SWITCH1  6
#define SWITCH2  2
#define SWITCH3  4
#define SWITCH4  5

#include <DmxSimple.h>

#include <SPI.h>
#include <Ethernet.h>

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

// vars to hold state
int dmx[] = {0,0,0,0,0,0,0,0,0,0,0,0};


void switch_channel(int channel) {
  if (dmx[channel] > 0) {
    DmxSimple.write(channel, 0);
    dmx[channel] = 0;
    Serial.print("Switch Off: ");
    Serial.println(channel);
  } else {
    DmxSimple.write(channel, 255);
    dmx[channel] = 255;
    Serial.print("Switch On: ");
    Serial.println(channel);
  } 
}

void debounce(int channel) {
  while (digitalRead(channel) == HIGH) {
    delay(100);
  }
  delay(100);
}
  
  
void check_switches() {
  // debounce missing
  if (digitalRead(SWITCH1) == HIGH) { switch_channel(LIGHT1); debounce(SWITCH1); }
  if (digitalRead(SWITCH2) == HIGH) { switch_channel(LIGHT2); debounce(SWITCH2); }
  if (digitalRead(SWITCH3) == HIGH) { switch_channel(LIGHT3); debounce(SWITCH3); }
  if (digitalRead(SWITCH4) == HIGH) { switch_channel(LIGHT4); debounce(SWITCH4); }
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
  
  pinMode(SWITCH1, INPUT); 
  pinMode(SWITCH2, INPUT); 
  pinMode(SWITCH3, INPUT); 
  pinMode(SWITCH4, INPUT); 
  
}


void loop() { 
  check_switches();

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK"); 
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          // output the value of each analog input pin
          for (int switch_value = 0; switch_value < sizeof(dmx); switch_value++) {
            
            client.print("switch ");
            client.print(switch_value);
            client.print(" is ");
            client.print(dmx[switch_value]);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
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
