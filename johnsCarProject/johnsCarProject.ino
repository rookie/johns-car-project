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

int debug_demo_mode = 0;
int debug_fast_baud = 1;
int debug_fast_update = 1;
int debug_show_gps_fix = 1;

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

#define gpsPin 3

#include <Time.h>

//#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_ILI9340.h> // 2.2inch
//Adafruit_ILI9340 tft = Adafruit_ILI9340(cs, dc, rst);

#include <PDQ_GFX.h>				// PDQ: Core graphics library
#include "PDQ_ILI9340_config.h"			// PDQ: ST7735 pins and other setup for this sketch
#include <PDQ_ILI9340.h>			// PDQ: Hardware-specific driver library
PDQ_ILI9340 tft;				// PDQ: create LCD object (using pins in "PDQ_ST7735_config.h")


#include <SPI.h>
//GPS
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#define COLOR_WHITE ILI9340_WHITE
#define COLOR_BLACK ILI9340_BLACK
#define COLOR_RED   ILI9340_RED  
#define COLOR_GREEN   ILI9340_GREEN  
#define COLOR_YELLOW ILI9340_YELLOW
#define COLOR_BLUE ILI9340_BLUE

//GPS
// If using software serial, keep these lines enabled
// (you can change the pin numbers to match your wiring):
SoftwareSerial mySerial(3, 2); //2 is tx 3 is overridden HIGH by gpsPin

#define DSerial mySerial
//#define DSerial Serial


//Serial = NULL;

//Adafruit_GPS GPS(&mySerial);
// If using hardware serial (e.g. Arduino Mega), comment
// out the above six lines and enable this line instead:
Adafruit_GPS GPS(&Serial);


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy


uint16_t fgColor = COLOR_WHITE;
uint16_t bgColor = COLOR_BLACK;
typedef enum {IMPERIAL, METRIC, NONE} displayMode_t;
displayMode_t displayMode = IMPERIAL;
int offsetDST = 0;

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
  
  
//14400
//19200
//28800
//38400
  DSerial.begin(57600);
  DSerial.println("setup");

  setupLCD();

  tft.setRotation(3);
  //fgColor = tft.Color565(255, 165, 0);
  //fgColor = tft.Color565(255, 127, 0);
  //DSerial.print("fgColor: 0x");
  //DSerial.println(fgColor, HEX);
  
  tft.setTextColor(fgColor, bgColor);
  

  //Mode changing
  int toggleLeft_GST = digitalRead(4);
  int toggleRight_MPH = digitalRead(7);
  if(toggleLeft_GST == LOW)
  {
    DSerial.println("Set GST Offset");
    toggleDSTOffset();
  } else 
  
  if (toggleRight_MPH == LOW)
  { 
    DSerial.println("Toggle MPH/KPH");
    toggleMPHMode();
  }
  
  
  if (getDSTOffset() == 0) {
    offsetDST = 0;
  } else {
    offsetDST = 1;
  }
  
  if (getMPHMode() == 0 || debug_demo_mode) {
    displayMode = IMPERIAL;
  } else {
    displayMode = METRIC;
  }
  
  setupGPS();

  //DEBUG
  //drawDebugUI();
  drawInitialUI();
  DSerial.println("setup done");
}

#ifndef PMTK_API_SET_FIX_CTL_1HZ
#define PMTK_API_SET_FIX_CTL_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_API_SET_FIX_CTL_5HZ  "$PMTK300,200,0,0,0,0*2F"
#endif

//http://www.hhhh.org/wiml/proj/nmeaxor.html
#define PMTK_SET_BAUD_14400  "$PMTK251,14400*29"
#define PMTK_SET_BAUD_19200  "$PMTK251,19200*22"
//#define PMTK_SET_BAUD_28800  "$PMTK251,28800*2A"
#define PMTK_SET_BAUD_38400  "$PMTK251,38400*27"


void setupGPS()
{
  DSerial.println("setupGPS()");
  
  //Enable GPS
  pinMode(gpsPin, OUTPUT);
  digitalWrite(gpsPin, HIGH);
  delay(1000);

  if(debug_fast_baud)
  {
    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
    GPS.begin(9600);
    DSerial.println("faster baud");
    GPS.sendCommand(PMTK_SET_BAUD_57600);
    delay(500);
    GPS.begin(57600);
    delay(500);
    DSerial.println("faster baud done");
  } else {
    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
    GPS.begin(57600);
    DSerial.println("slower baud");
    GPS.sendCommand(PMTK_SET_BAUD_9600);
    delay(500);
    GPS.begin(9600);
    delay(500);
    DSerial.println("slower baud done");
  }
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  if(debug_fast_update) {
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);   // 5 Hz update rate
    GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);
  } else {
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
    GPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ);
  }
  
  
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Turn off updates on antenna status
  GPS.sendCommand(PGCMD_NOANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  delay(1000);
  // Ask for firmware version
  //GPS.sendCommand(PMTK_Q_RELEASE);
  
  
  DSerial.println("setupGPS() done");
}

void setupLCD()
{
  
  //HACK: new stuff from GFX library, should probably be in library
#if defined(ILI9340_RST_PIN)
  FastPin<ILI9340_RST_PIN>::setOutput();
  FastPin<ILI9340_RST_PIN>::hi();
  FastPin<ILI9340_RST_PIN>::lo();
  delay(1);
  FastPin<ILI9340_RST_PIN>::hi();
#endif

  tft.begin();
  //HACK: new stuff from GFX library, should probably be in library
  SPI.setClockDivider(SPI_CLOCK_DIV2);	// 8 MHz (full! speed!) [1 byte every 18 cycles]

  DSerial.println("init");

  uint16_t time = millis();
  tft.fillScreen(bgColor);
  time = millis() - time;
  DSerial.println(time, DEC);
  
  pinMode(light, OUTPUT);
  digitalWrite(light, HIGH);
  //analogWrite(light, 128);
}


void loop() {
  gpsLoop();
  tempReadLoop();
  
  //Tests
  //testUIBars();
  
  if(debug_demo_mode) {
    testUISpeed();
    testUIDateTime();
  }

  calculateUI();
  
  if(debug_demo_mode) {
    testUITemp(); //Must run after calculateUI()
  }
  
  updateUI();
  //delay(100);
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
  static int inOutTemp = -19;
  
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
  if(bartest >= 318 || bartest <= 0){
    direction = -direction;
  }
  
  static int smallbartest = 0;
  static int smalldirection = 1;
 
  //drawUILeftBar(smallbartest, COLOR_WHITE);
  //drawUIRightBar(smallbartest, COLOR_WHITE);
  
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
  if(speedtest >= 110 || speedtest <= 0){
    direction = -direction;
  }
  
  //delay(250);
}

void testUIDateTime()
{
  static int dateMonth = 0;
  static int dateDay   = 0;
  static int timeHour  = 0;
  static int timeMin   = 0;
  
  
  dateMonth++;
  dateDay++;
  timeHour++;
  timeMin++;
  
  if(dateMonth > 12) dateMonth = 1;
  if(dateDay   > 31) dateDay = 1;
  if(timeHour  > 12) timeHour = 1;
  if(timeMin   > 59) timeMin = 0;
  
  
  
  sensorValues.month = dateMonth;
  sensorValues.day   = dateDay;
  sensorValues.hour  = timeHour;
  sensorValues.min   = timeMin;
  
  sensorsUpdated.month = 1;
  sensorsUpdated.day = 1;
  sensorsUpdated.hour = 1;
  sensorsUpdated.min = 1;
  
}

void calculateUI()
{
  int celsius = 0;
  
  //The plus 4 is to minimize rounding errors (half of 9 = 4.5)
  if(sensorsUpdated.coolantValue != 0){
    displayF.coolantTempValue = ((69529.0 - 9334.0 * log(sensorValues.coolantValue)) / 100.0);
    sprintf(displayF.coolantTemp, "%3d", displayF.coolantTempValue);  
    celsius = ((displayF.coolantTempValue - 32)*5+4)/9;
    sprintf(displayC.coolantTemp, "%3d", celsius);  
  }
  if(sensorsUpdated.outsideValue != 0){
    displayF.outsideTempValue = ((90000.0 - (100.0 * sensorValues.outsideValue)) / 619.0);
    sprintf(displayF.outsideTemp, "%3d", displayF.outsideTempValue); 
    celsius = ((displayF.outsideTempValue - 32)*5+4)/9; 
    sprintf(displayC.outsideTemp, "%3d", celsius);  
  }
  if(sensorsUpdated.insideValue != 0){
    displayF.insideTempValue = ((90000.0 - (100.0 * sensorValues.insideValue)) / 619.0);
    sprintf(displayF.insideTemp, "%3d", displayF.insideTempValue);  
    celsius = ((displayF.insideTempValue - 32)*5+4)/9;
    sprintf(displayC.insideTemp, "%3d", celsius);
  }
  
  if (sensorsUpdated.speedKnots != 0) {
    
    //Serial.print("Speed (knots): "); Serial.println(sensorValues.speed);
    //Serial.print("Speed   (mph): "); Serial.println(sensorValues.speed * 1.15078);
    //Serial.print("Speed   (kph): "); Serial.println(sensorValues.speed * 1.852);
    
    //TODO: support 100+ numbers
    if(sensorValues.speedMPH > 99) sensorValues.speedMPH = 99;
    if(sensorValues.speedKPH > 99) sensorValues.speedKPH = 99;
    
    //Hide low values
    if(sensorValues.speedMPH <= 2) sensorValues.speedMPH = 0;
    if(sensorValues.speedKPH <= 3) sensorValues.speedKPH = 0;
    
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
  if (sensorAnalogValue != sensorValues.coolantValue &&
      (sensorAnalogValue != sensorValues.coolantValue + 1) &&
      (sensorAnalogValue != sensorValues.coolantValue - 1))
  {
    sensorValues.coolantValue = sensorAnalogValue;
    sensorsUpdated.coolantValue = 1;
  }
  sensorAnalogValue = analogRead(tempOutsidePin);
  if (sensorAnalogValue != sensorValues.outsideValue &&
      (sensorAnalogValue != sensorValues.outsideValue + 1) &&
      (sensorAnalogValue != sensorValues.outsideValue - 1))
  {
    sensorValues.outsideValue = sensorAnalogValue;
    sensorsUpdated.outsideValue = 1;
  }
  sensorAnalogValue = analogRead(tempInsidePin);
  if (sensorAnalogValue != sensorValues.insideValue &&
      (sensorAnalogValue != sensorValues.insideValue + 1) &&
      (sensorAnalogValue != sensorValues.insideValue - 1))
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
  char *lastNMEA;
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) DSerial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    lastNMEA = GPS.lastNMEA();
    if (!GPS.parse(lastNMEA))   // this also sets the newNMEAreceived() flag to false
    {
      DSerial.println("GPS parse failed");
      return;  // we can fail to parse a sentence in which case we should just wait for another
    }
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 0.5 seconds or so, print out the current stats
  if (millis() - timer > 200) {
    DSerial.print("has fix = ");
    DSerial.println(GPS.fix);
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
      time_t rawTime = now();
      
      /*
      DSerial.println("================");
      DSerial.print(GPS.hour);
      DSerial.print(":");
      DSerial.println(GPS.minute);
      DSerial.print(GPS.month);
      DSerial.print("/");
      DSerial.print(GPS.day);
      DSerial.print("/");
      DSerial.println(2000+GPS.year);
      DSerial.println("==--------------");
      */
      
      
      DSerial.println("====");
      DSerial.print(hour());
      DSerial.print(":");
      DSerial.println(minute());
      DSerial.print(hour(rawTime));
      DSerial.print(":");
      DSerial.println(minute(rawTime));
      
      int GSTOffset = -8 + offsetDST;
      //adjustTime(-7*60*60); //PDT
      //adjustTime(-8*60*60); //PST
      adjustTime(GSTOffset*60*60);
      rawTime = now();
      
      DSerial.println("----");
      DSerial.print(hour());
      DSerial.print(":");
      DSerial.println(minute());
      DSerial.print(hour(rawTime));
      DSerial.print(":");
      DSerial.println(minute(rawTime));
        
      
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

