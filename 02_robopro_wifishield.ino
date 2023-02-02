/*
MQTT Connection using WiFi Shield 
Original code from https://github.com/zakibakar75/MyMQTT_Dioty_Connection_JSON_ESP8266_WiFiESP_library
*/

#include "WiFiEsp.h"
#include <PubSubClient.h>

// Emulate Serial1 on pins 2/3 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(2, 3); // RX, TX
#endif

WiFiEspClient client;
PubSubClient mqttClient(client);



//VARIABLES
int val1 = 0.0;           // variable to store the value coming from the sensor
int val2 = 0.0;           // variable to store the value coming from the sensor
unsigned long lastMillis = 0;

const char* server = "broker.hivemq.com"; //MQTT server (of your choice)
char ssid[] = "your_wifi_ssid";              // your network SSID (name)
char pass[] = "your_wifi_password";               // your network password
int status = WL_IDLE_STATUS;              // the Wifi radio's status

#define BUILTIN_LED 13
#define MQTT_PUBLISH_TOPIC    "RoboPRO_YourPhoneNo/iot/weather"
#define MQTT_SUBSCRIBE_TOPIC1    "RoboPRO_YourPhoneNo/iot/led1"
#define MQTT_SUBSCRIBE_TOPIC2    "RoboPRO_YourPhoneNo/iot/led2"
/***********************************************************************/


void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  pinMode(BUILTIN_LED, OUTPUT);
  
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    //while (true);
  }  

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network!"); 

  delay(1500);
  mqttClient.setServer(server, 1883);
  mqttClient.setCallback(callback);
  connecttomqtt();
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(13, LOW);   // Turn the LED on 
    // (Note that LOW is the voltage level but actually the LED is on; 
    // this is because it is acive low on the ESP-01
  } else {
    digitalWrite(13, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void connecttomqtt() {

  Serial.println("+++++ Connect to MQTT server >>>");
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(NULL,NULL,NULL)) {
      Serial.print("--> Connected to ");
      Serial.println(server);
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

void loop() {

    if (!mqttClient.connected()) {
        Serial.println("MQTT disconnected! <<<");
        connecttomqtt();
    }

    mqttClient.loop();
    delay(10);  // <- fixes some issues with WiFi stability
     
  // publish sensor reading every 10 seconds   
  if (millis() - lastMillis > 10000) { 
      lastMillis = millis();

      char *msg = "{\"temperature\":1,\"led1\":2}";
      
      Serial.println();
      Serial.println("Publish message : ");
      Serial.println(msg);
      mqttClient.publish(MQTT_PUBLISH_TOPIC, msg);

    }

  delay(2000); 
}