/*
Arduino Training Series by Cikgu Fadzli
IoT with Node-RED
MQTT Connection using WiFi Shield 
This code can be downloaded from https://github.com/efadzli/arduino_iot_wifishield
*/

int temp_pin= A0;
int led1_pin= 13;
int led2_pin= 12;

void setup(void)
{ 
  Serial.begin(9600);
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, LOW);
}

void loop() {  
  int analogValue = analogRead(temp_pin); // get reading from LM35 temperature sensor
  float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by UNO
  float celsius = millivolts/10;

  Serial.print("Temperature = ");
  Serial.print(celsius);
  Serial.println(" °C");

  if(celsius>30){ // if temperature is more than 30, Red LED on (you can change this value accordingly
    digitalWrite(led1_pin, LOW);
    digitalWrite(led2_pin, HIGH);
  } else { // if temperature is 30 or less, Green LED on
    digitalWrite(led1_pin, HIGH);
    digitalWrite(led2_pin, LOW);
  }
  delay(1000); // wait for 1 second
}
