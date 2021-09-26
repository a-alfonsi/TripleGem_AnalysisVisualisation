/*
  HYT939 sensor with Arduino UNO:
  Sensor pin 4 = SDA = Arduino pin A4
  Sensor pin 1 = SCL = Arduino pin A5
  Sensor pin  3 = Arduino ground
  Sensor pin 2 = VCC = Arduino power 3.3V
  Two resistors, see scheme
 */


 // Variables and libraries
 #include <Wire.h>        // I2C protocol library
 #define ADDR 0x28        // HYT939 I2C protocol address (see datasheet)
 unsigned int tempraw;    // raw temperature value read from sensor
 unsigned int humraw;     // raw humidity value read from sensor
 double temp;             // final measured temperature
 double hum;              // final measured humidity
 int x;                   // loop variable
 unsigned char buffer[4]; // buffer to hold data read from sensor
  

 void setup()
 {
  // Initialise I2C communication
  Wire.begin();
  // Initialise serial communication
  Serial.begin(9600);
 }

  
 void loop()
 {
  // I2C transmission
  Wire.beginTransmission(ADDR);
  Wire.endTransmission();
  delay(200);
  
  // Read 4 bytes of data, organised as follows (see datasheet):
  // [ hum byte1, hum byte2, temp byte1, temp byte2 ]
  Wire.requestFrom(ADDR, 4, true);
  x=0;
  while(Wire.available())
  { 
    char c = Wire.read(); 
    buffer[x]=c; 
    x++; 
  }
  
  // Combine temperature bytes to calculate raw temperature
  tempraw=buffer[2]*256+buffer[3];

  // Combine humidity bytes to calculate raw humidity
  humraw=buffer[0]*256+buffer[1];
  
  // Assign temperature and humidity to respective addresses
  // and mask away 2 least significant bits (see datasheet)
  tempraw&=0xfffc;
  humraw&=0x3fff;
  tempraw=tempraw/4; 
  
  // Convert raw values to user friendly values (see datasheet)
  temp=(double)tempraw/99.2909; 
  temp=temp-40.0;
  hum=(double)humraw/163.83;

  // Print values
  Serial.print(F("Humidity: "));
  Serial.print(hum);
  Serial.print(F("% Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C"));
  Serial.print("\n");
  delay(1000);
 }

 