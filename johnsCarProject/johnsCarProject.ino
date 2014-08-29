/***************************************************
  This is an example sketch for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutoriajohns-car-projectls and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

//TODO: add string term \0 to UIvalues, somebody will print them someday

#define tempCoolantPin A0
#define tempOutsidePin A1
#define tempInsidePin  A2

// For the breakout, you can use any (2 or) 3 pins
//#define sclk 13
//#define mosi 11
#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset

#define light 5
//Use these pins for the shield!
//#define cs   10
//#define dc   8
//#define rst  0  // you can also connect this to the Arduino reset

#include <Time.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9340.h> // 2.2inch

#include <SPI.h>
//GPS
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// Option 1: use any pins but a little slower
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)

Adafruit_ILI9340 tft = Adafruit_ILI9340(cs, dc, rst);

#define COLOR_WHITE ILI9340_WHITE
#define COLOR_BLACK ILI9340_BLACK
#define COLOR_RED   ILI9340_RED  
#define COLOR_YELLOW ILI9340_YELLOW
#define COLOR_BLUE ILI9340_BLUE

//GPS
// If using software serial, keep these lines enabled
// (you can change the pin numbers to match your wiring):
SoftwareSerial mySerial(3, 2);

Adafruit_GPS GPS(&mySerial);
// If using hardware serial (e.g. Arduino Mega), comment
// out the above six lines and enable this line instead:
//Adafruit_GPS GPS(&Serial1);


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy


uint16_t fgColor = COLOR_WHITE;
uint16_t bgColor = COLOR_BLACK;

struct values {
   uint16_t coolantValue;
   uint16_t outsideValue;
   uint16_t insideValue;
   
   uint8_t fix;
   //These might all come over as strings
   float speedKnots; //Knots
   int speedMPH; //MPH
   int speedKPH; //KPH
   uint8_t hour;
   uint8_t min;
   uint8_t day;
   uint8_t month;
};
struct displayValues {
   char coolantTemp[4];
   char outsideTemp[4];
   char insideTemp[4];
   int16_t coolantTempValue;
   int16_t outsideTempValue;
   int16_t insideTempValue;
   char speed[4];
   char time[6];  //12:02
   char date[6];  //12/13 or 12/12
};

struct values sensorValues;
struct values sensorsUpdated; //0 for not changed, 1 for changed
struct displayValues displayF;
struct displayValues displayC;

void setup(void) {
  Serial.begin(9600);
  Serial.println("hello!");

  setupLCD();

  tft.setRotation(3);
  //fgColor = tft.Color565(255, 165, 0);
  //fgColor = tft.Color565(255, 127, 0);
  //Serial.print("fgColor: 0x");
  //Serial.println(fgColor, HEX);
  
  tft.setTextColor(fgColor, bgColor);
  

  setupGPS();

  drawInitialUI();

  Serial.println("done");
}

void setupGPS()
{
  
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_NOANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
  
}

void setupLCD()
{

  tft.begin();

  Serial.println("init");

  uint16_t time = millis();
  tft.fillScreen(bgColor);
  time = millis() - time;
  Serial.println(time, DEC);
  
  pinMode(light, OUTPUT);
  digitalWrite(light, HIGH);
  //analogWrite(light, 128);
}


void loop() {
  gpsLoop();
  tempReadLoop();
  
  //Tests
  testUISpeed();
  //testUIBars();
  //testUITemp(); //Must comment out calculateUI() to run these
  
  calculateUI();
  updateUI();
  delay(100);
}

void testUITemp()
{
  //115-275
  static int coolantTemp = 100;
  static int direction = 1;
  
  coolantTemp += direction;
  displayF.coolantTempValue = coolantTemp;
  sprintf(displayF.coolantTemp, "%3d", displayF.coolantTempValue);  
  sensorsUpdated.coolantValue = 1;
  
  
  //32-106
  static int inOutTemp = 0;
  
  inOutTemp += direction;
  displayF.outsideTempValue = inOutTemp;
  sprintf(displayF.outsideTemp, "%3d", displayF.outsideTempValue);  
  sensorsUpdated.outsideValue = 1;  

  displayF.insideTempValue = inOutTemp;
  sprintf(displayF.insideTemp, "%3d", displayF.insideTempValue);  
  sensorsUpdated.insideValue = 1;  
  
  if(inOutTemp >= 175 || inOutTemp <= -20){
    direction = -direction;
  }
}

void testUIBars()
{
  static int bartest = 0;
  static int direction = 1;
  drawUITopBar(bartest, COLOR_RED);
  
  bartest += direction;
  if(bartest >= 148 || bartest <= 0){
    direction = -direction;
  }
  
  static int smallbartest = 0;
  static int smalldirection = 1;
 
  drawUILeftBar(smallbartest, COLOR_WHITE);
  drawUIRightBar(smallbartest, COLOR_WHITE);
  
  smallbartest += smalldirection;
  if(smallbartest >= 68 || smallbartest <= 0){
    smalldirection = -smalldirection;
  }
  
  
}

void testUISpeed()
{
  static int speedtest = 0;
  static int direction = 1;
  
  sensorValues.speedMPH = speedtest;
  sensorsUpdated.speedKnots = 1;
  speedtest += direction;
  if(speedtest >= 199 || speedtest <= 0){
    direction = -direction;
  }
  
  delay(250);
}

void calculateUI()
{
  if(sensorsUpdated.coolantValue != 0){
    displayF.coolantTempValue = ((69529.0 - 9334.0 * log(sensorValues.coolantValue)) / 100.0);
    sprintf(displayF.coolantTemp, "%3d", displayF.coolantTempValue);  
    sprintf(displayC.coolantTemp, "%3d", 100);  
  }
  if(sensorsUpdated.outsideValue != 0){
    displayF.outsideTempValue = ((90000.0 - (100.0 * sensorValues.outsideValue)) / 619.0);
    sprintf(displayF.outsideTemp, "%3d", displayF.outsideTempValue);  
    sprintf(displayC.outsideTemp, "%3d", -40);  
  }
  if(sensorsUpdated.insideValue != 0){
    displayF.insideTempValue = ((90000.0 - (100.0 * sensorValues.insideValue)) / 619.0);
    sprintf(displayF.insideTemp, "%3d", displayF.insideTempValue);  
    sprintf(displayC.insideTemp, "%3d", 32);  
  }
  
  if (sensorsUpdated.speedKnots != 0) {
    
    //Serial.print("Speed (knots): "); Serial.println(sensorValues.speed);
    //Serial.print("Speed   (mph): "); Serial.println(sensorValues.speed * 1.15078);
    //Serial.print("Speed   (kph): "); Serial.println(sensorValues.speed * 1.852);
    
    //TODO: support 100+ numbers
    if(sensorValues.speedMPH > 99) sensorValues.speedMPH = 99;
    if(sensorValues.speedKPH > 99) sensorValues.speedKPH = 99;
    sprintf(displayF.speed, "%2d", sensorValues.speedMPH); 
    sprintf(displayC.speed, "%2d", sensorValues.speedKPH); 
  }
  if (sensorsUpdated.min != 0) {  
    sprintf(displayF.time, "%2d:%02d", sensorValues.hour, sensorValues.min);  
    sprintf(displayC.time, "%2d:%02d", sensorValues.hour, sensorValues.min);  
  }
  if (sensorsUpdated.day != 0) {
    sprintf(displayF.date, "%2d/%2d", sensorValues.month, sensorValues.day);  
    sprintf(displayC.date, "%2d/%2d", sensorValues.day, sensorValues.month);  
  }
}

void tempReadLoop()
{
  int sensorAnalogValue = analogRead(tempCoolantPin);
  if (sensorValues.coolantValue != sensorAnalogValue)
  {
    sensorValues.coolantValue = sensorAnalogValue;
    sensorsUpdated.coolantValue = 1;
  }
  sensorAnalogValue = analogRead(tempOutsidePin);
  if (sensorValues.outsideValue != sensorAnalogValue)
  {
    sensorValues.outsideValue = sensorAnalogValue;
    sensorsUpdated.outsideValue = 1;
  }
  sensorAnalogValue = analogRead(tempInsidePin);
  if (sensorValues.insideValue != sensorAnalogValue)
  {
    sensorValues.insideValue = sensorAnalogValue;
    sensorsUpdated.insideValue = 1;
  }
  
}

#pragma mark - GPS Callbacks


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();
void gpsLoop()
{
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 0.5 seconds or so, print out the current stats
  if (millis() - timer > 500) {
    timer = millis(); // reset the timer
    /*
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", "); 
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
    /**/
    
    if (sensorValues.min != GPS.minute)
    {
      //TODO: this is UTC time
      setTime(GPS.hour, GPS.minute, 0, GPS.day, GPS.month, GPS.year);
      Serial.println(GPS.year);
      time_t rawTime = now();
      /*
      Serial.println("================");
      Serial.println(hour());
      Serial.println(minute());
      Serial.println("----------------");
      Serial.println(hour(rawTime));
      Serial.println(minute(rawTime));
      Serial.println("==--------------");
      */
      adjustTime(-7*60*60);
      rawTime = now();
      /*
      Serial.println("----------------");
      Serial.println(hour());
      Serial.println(minute());
      Serial.println("----------------");
      Serial.println(hour(rawTime));
      Serial.println(minute(rawTime));
      Serial.println("");
      */
    
    if (sensorValues.month != month(rawTime) || sensorValues.day != day(rawTime))
    {
      sensorValues.month = month(rawTime);
      sensorValues.day   = day(rawTime);
      
      sensorsUpdated.month = 1;
      sensorsUpdated.day = 1;
    }

    if (sensorValues.hour != hour(rawTime) || sensorValues.min != minute(rawTime))
    {
      sensorValues.hour  = hour(rawTime);
      sensorValues.min   = minute(rawTime);
      if (sensorValues.hour > 12) sensorValues.hour -= 12;
      
      sensorsUpdated.hour = 1;
      sensorsUpdated.min = 1;
    }
    
    }
    //TODO: Just assume this is always updating?
    //TODO: Should at least calculate mph values to compare
    sensorValues.fix   = GPS.fix;
    if (GPS.fix){
      
      if (sensorValues.speedKnots != GPS.speed)
      {
        sensorValues.speedKnots = GPS.speed;
        sensorValues.speedMPH = (int)((sensorValues.speedKnots * 1.15078) + 0.5); //mph round up 
        sensorValues.speedKPH = (int)((sensorValues.speedKnots * 1.852) + 0.5);  //kph round up 
 
        sensorsUpdated.speedKnots = 1;
      }
    }
  }
}

