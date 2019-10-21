#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include "globals.h"

//JSON
StaticJsonDocument<300> doc;

//Millis
unsigned long PREV_TIME = 0;
const long DURATION = 5000;
const long PERIOD = 10000;

//Wifi
WiFiClient espClient;

//MQTT
PubSubClient client(espClient);

//Ultra
long duration, inches, distance;

//Door Status
bool door_status = false;
bool door_close_sent = true;

//Callback for the garage sensor
void callback(char *topic, byte *payload, unsigned int length) {
  static char message[MAX_MSG_LEN+1];
  if (length > MAX_MSG_LEN) {
    length = MAX_MSG_LEN;
  }
  strncpy(message, (char *)payload, length);
  message[length] = '\0';
  Serial.println(topic);
  Serial.println(message);
  Serial.println(length);

  if(strcmp(message, "open") == 0){
    door_status = true;
  }
  else{
    door_status = false;
    door_close_sent = false;
  }
}

void setup() {
  //Serial Port begin
  Serial.begin (9600);

  //Define inputs and outputs
  pinMode(D7, OUTPUT);
  pinMode(D6, INPUT);

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
  client.setCallback(callback);

}

void connect_mqtt() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266 Ultra")) {
      client.subscribe(door_sensor_topic);
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
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    connect_mqtt();
  }
  client.loop();
  if(door_status){
    distance = calculate();
    doc["data"] = distance;
    char buffer[512];
    size_t n = serializeJson(doc, buffer);
    client.publish(light_topic, buffer, n);
  }
  if(!door_close_sent && !door_status){
    doc["data"] = -1;
    char buffer[512];
    size_t n = serializeJson(doc, buffer);
    client.publish(light_topic, buffer, n);
    door_close_sent = true;
  }
}

long calculate() {
  // Open the sensor for 10 ms
  digitalWrite(D7, LOW);
  delayMicroseconds(5);
  digitalWrite(D7, HIGH);
  delayMicroseconds(10);
  digitalWrite(D7, LOW);

  // Duration of the echo pulse
  duration = pulseIn(D6, HIGH);
 
  // Convert the time into a distance
  inches = (duration/2) / 74;
  
  delay(500);
  return inches;
}
