#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

wl_status_t wifi_status;
wl_status_t wifi_status_prev;
long lastReconnectAttempt = 0;

// MQTT callback for new messages
// Should check whether topic matches but couldn't make that work
// So it doesn't check, but that should be ok since we're only subscribed on one topic
// for now at least
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

//  if (strcmp(topic,MQTT_TOPIC_SUBSCRIBE)) {
  //  Serial.println("Topic matches");
    payload[length] = 0;
    dmxInHandler((char*)payload, length);
  //}
}

bool mqtt_connect() {
  if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("MQTT connected");
    IPAddress localip = WiFi.localIP();
    char localip_c[16];
    sprintf(localip_c,"%u.%u.%u.%u", localip[0], localip[1], localip[2], localip[3]);
    client.publish(MQTT_TOPIC_CONTROLLER, localip_c);
    client.subscribe(MQTT_TOPIC_SUBSCRIBE);
    return client.connected();
  }
  return false;
}

void setup_mqtt() {
  setup_wifi();
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

// Setup wifi credentials
// Does not wait for connection
void setup_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PSK);
}

// Default network loop
// Reacts on changes of network status
// Checks for new MQTT messages
void loop_mqtt() {
  wifi_status = WiFi.status();

  // Check whether WiFi Status has changed
  if (wifi_status != wifi_status_prev) {
    
    // Wifi Status has changed
    if (wifi_status == WL_CONNECTED) {
      // we are now connected to the wifi (again)
      Serial.print("WiFi connected, ");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else if (wifi_status_prev == WL_CONNECTED) {
      Serial.println("Wifi connection lost");
    }
    wifi_status_prev = wifi_status;
  }

  // Reconnect to MQTT broker if we are connected wifi, but not to the broker
  if (!client.connected() && wifi_status == WL_CONNECTED) {
    
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      Serial.println("Attempting MQTT reconnect");
      // Attempt to reconnect
      if ( mqtt_connect()) {
        lastReconnectAttempt = 0;
      }
    }
  }  

  // MQTT client loop, handles new messages etc.
  client.loop();
}

// Send current DMX state to MQTT topic
void updateStatesEntry() {
  String strStates;
  for( int i = 0; i < intMaxChannel; i++ ) {
    strStates = strStates + i + ":" + dmxChannels[i] + ",";
  } 
  strStates.remove(strStates.length() - 1);
  char state[strStates.length() + 1];
  strStates.toCharArray(state,strStates.length());
  // set mqtt state msg with retain set to true
  if( client.publish(MQTT_TOPIC_STATE, state, strStates.length() + 1)) {
    Serial.print("Successfully published ");
  } else {
    Serial.print("Failed publishing ");
  }
  Serial.print("State: ");
  Serial.println(strStates.c_str());
}
