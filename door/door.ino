#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include "globals.h"

//Wifi
WiFiClient espClient;

//MQTT
PubSubClient client(espClient);

void setup() {
  //Serial Port begin
  Serial.begin (9600);

  //Define inputs and outputs

  // WIFI Info and connect
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Waiting to connect…");
  }

  // IP Addresses
  Serial.print("MY IP address: ");
  Serial.println(WiFi.localIP());

  //MQTT PUB/SUB
  client.setServer(mqtt_hostname, 1883);
}

void connect_mqtt() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266 Client")) {
      Serial.println("Connected");
      } 
    else {
      Serial.println("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    connect_mqtt();
  }
  client.loop();
  client.publish(periodic_topic, "closed");
  delay(5000);






  
}
