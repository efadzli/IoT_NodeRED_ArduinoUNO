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

  digitalWrite(led1_pin, HIGH);
  digitalWrite(led2_pin, LOW);
  delay(500);
  digitalWrite(led1_pin, LOW);
  digitalWrite(led2_pin, HIGH);
  delay(500);
}



