#ifndef mqtt_h
#define mqtt_h

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

#include <WiFiClient.h>
#include <PubSubClient.h>
#include "config.h"

bool mqttNewMessage = false;
String mqttTopic   = "";
String mqttPayload = "";

IPAddress server(0, 0, 0, 0);

WiFiClientSecure client;  // with TLS 
//WiFiClient client;      // without TLS 
PubSubClient mqttClient(client, server);

// handle received serial data
void publishMQTTMessage(String strTopic, String strMessage, bool bRetain=false) {
  // publish mqtt message with given topic
  if (bRetain)
    mqttClient.publish(MQTT::Publish(strTopic, strMessage).set_retain().set_qos(1));
  else
    mqttClient.publish(strTopic, strMessage);
  DEBUG_PRINT("send MQTT: topic='" + strTopic + "', message='" + strMessage + "', retain=");
  DEBUG_PRINTLN(bRetain);
}

void subscribeMQTTTopic(String strTopic) {
  mqttClient.subscribe(strTopic);
  Serial.println("subscribe MQTT: topic='" + strTopic + "'");
}

bool connectMQTT(String strUser, String strPass, String strHost, uint16_t port = mqtt_port) {
  // connect to mqtt broker
  mqttClient.set_server(strHost, port);
  bool MQTTconnected = false;
  int retries = 0;
  while(!MQTTconnected && retries < 10) {
    retries++;
    //String strClientID = String(mqtt_client_id);
    if (mqttClient.connect(MQTT::Connect(mqtt_client_id).set_auth(strUser, strPass).set_will(strTopicPrefixID + "controller", "", 1, true))) {
      MQTTconnected = true;
    }
    delay(10);
  }
  return MQTTconnected;
}

bool processMQTTLoop() {
  // handle mqtt messages and wifi connection
  if (mqttClient.connected()) {
    mqttClient.loop();
    return true;
  }
  return false;
}



#endif //mqtt_h
