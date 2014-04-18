/*
  ReadAnalogVoltage
  Reads an analog input on pin 0, converts it to voltage, and prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
 This example code is in the public domain.
 */
 

int sensorPin1 = A0;
int sensorPin2 = A1;
int sensorPin3 = A2;

int sensorValue1 = 0;
int sensorValue2 = 0;
int sensorValue3 = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int updated = 0;
  
  int newSensorValue1 = analogRead(sensorPin1);
  int newSensorValue2 = analogRead(sensorPin2);
  int newSensorValue3 = analogRead(sensorPin3);
  
  if (newSensorValue1 != sensorValue1) updated = 1;
  if (newSensorValue2 != sensorValue2) updated = 1;
  if (newSensorValue3 != sensorValue3) updated = 1;
  
  if (updated)
  {
    sensorValue1 = newSensorValue1;
    sensorValue2 = newSensorValue2;
    sensorValue3 = newSensorValue3;
    
    Serial.print(sensorValue1);
    Serial.print("\t");
    Serial.print(sensorValue2);
    Serial.print("\t");
    Serial.print(sensorValue3);
    Serial.println();
    
  }
  delay(100);
}
