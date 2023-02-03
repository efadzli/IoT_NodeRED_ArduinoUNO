/*
MQTT Connection using WiFi Shield 
Original code from https://github.com/zakibakar75/MyMQTT_Dioty_Connection_JSON_ESP8266_WiFiESP_library
*/

#include "WiFiEsp.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Emulate Serial1 on pins 2/3 if not present
#ifndef HAVE_HWSERIAL1
  #include "SoftwareSerial.h"
  SoftwareSerial Serial1(2, 3); // RX, TX
#endif

WiFiEspClient client;
PubSubClient mqttClient(client);

//VARIABLES
#define WIFI_SSID             "ENTER_YOUR_WIFI_SSID_HERE"
#define WIFI_PASSWORD         "ENTER_YOUR_WIFI_PASSWORD_HERE"
#define MQTT_HOST             "broker.hivemq.com"
#define MQTT_PUBLISH_TOPIC    "RoboPRO_YourPhoneNo/iot/weather"
#define MQTT_SUBSCRIBE_TOPIC1    "RoboPRO_YourPhoneNo/iot/led1"
#define MQTT_SUBSCRIBE_TOPIC2    "RoboPRO_YourPhoneNo/iot/led2"

unsigned long lastMillis = 0;
int status = WL_IDLE_STATUS;      // the Wifi radio's status
int led1 = 13;
int led2 = 12;
int temperature = A0;
char msg[180];
StaticJsonDocument<200> doc;  
/***********************************************************************/

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, MQTT_SUBSCRIBE_TOPIC1) == 0){
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(led1, LOW);   // Turn the LED on by making the voltage LOW
    } else {
      digitalWrite(led1, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  } else if (strcmp(topic, MQTT_SUBSCRIBE_TOPIC2) == 0){
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(led2, LOW);   // Turn the LED on by making the voltage LOW
    } else {
      digitalWrite(led2, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  }
  // Note that LOW is the voltage level but actually the LED is on; 
  // this is because it is active low on the ESP-01  
}

void connecttomqtt() {

  Serial.println("Connect to MQTT server >>>");
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    // Attempt to connect
    if (mqttClient.connect(NULL,NULL,NULL)) {
      Serial.print("Connected to ");
      Serial.println(MQTT_HOST);
      // Once connected, subscribe to topic
      mqttClient.subscribe(MQTT_SUBSCRIBE_TOPIC1);
      mqttClient.subscribe(MQTT_SUBSCRIBE_TOPIC2);
    } else {
      Serial.print("[Failed] rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, LOW);  
  digitalWrite(led2, LOW);
  
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }  

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
  Serial.println();

  delay(1500);
  mqttClient.setServer(MQTT_HOST, 1883);
  mqttClient.setCallback(callback);
  connecttomqtt();
  Serial.println();
}

void loop() {

  if (!mqttClient.connected()) {
      Serial.println("MQTT disconnected! <<<");
      connecttomqtt();
  }

  mqttClient.loop();
  delay(100);  // <- fixes some issues with WiFi stability

  int analogValue = analogRead(temperature);
  float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by UNO
  float celsius = millivolts/10;

  int ledValue = digitalRead(led1);

  // publish sensor reading every 10 seconds   
  if (millis() - lastMillis > 10000) { 
      lastMillis = millis();

      doc["temperature"] = celsius;
      doc["led1"] = ledValue;      
      serializeJson(doc, msg);

      Serial.print("Publish to : ");
      Serial.print(MQTT_PUBLISH_TOPIC);
      Serial.println();

      Serial.print("Publish message : ");
      Serial.print(msg);
      Serial.println();

      mqttClient.publish(MQTT_PUBLISH_TOPIC, msg);
    }

  delay(2000); 
}