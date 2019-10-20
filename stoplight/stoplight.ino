void setup() {
  // put your setup code here, to run once:

}
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);
  if (receivedChar == '0')
  // ESP8266 Huzzah outputs are "reversed"
  digitalWrite(ledPin, HIGH);
  if (receivedChar == '1')
   digitalWrite(ledPin, LOW);
  }
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:

}
