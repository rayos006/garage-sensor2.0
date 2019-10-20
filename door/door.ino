#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include "globals.h"
//GPIO
const int button = 12;
void ICACHE_RAM_ATTR door_changed ();
//Millis
unsigned long PREV_TIME = 0;
const long DURATION = 5000;
const long PERIOD = 10000;

// Door state
//True = Open, False = Closed
bool door_state = false;

//Wifi
WiFiClient espClient;

//MQTT
PubSubClient client(espClient);

void door_changed(){
  Serial.println(door_state);
  if (door_state){
    client.publish(change_state_topic, "closed");
    door_state = false;
  }
  else {
    client.publish(change_state_topic, "open");
    door_state = true;
  }
}
  
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

  //Door state change
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(button, door_changed, CHANGE);
  
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
  unsigned long curr_time = millis();
  if (!client.connected()) {
    connect_mqtt();
  }
  client.loop();
  
  if(curr_time - PREV_TIME >= PERIOD) {
    if(door_state){
      client.publish(periodic_topic, "open");
    }
    else {
      client.publish(periodic_topic, "closed");
  }
  PREV_TIME = curr_time;
  }
}
