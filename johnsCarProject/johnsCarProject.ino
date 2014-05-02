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

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
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
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
float p = 3.1415926;

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


uint16_t fgColor = ST7735_WHITE;
uint16_t bgColor = ST7735_BLACK;

struct values {
   uint16_t coolantValue;
   uint16_t outsideValue;
   uint16_t insideValue;
   
   uint8_t fix;
   //These might all come over as strings
   float speed; //Knots
   uint8_t hour;
   uint8_t min;
   uint8_t day;
   uint8_t month;
   
};
struct displayValues {
   char coolantTemp[4];
   char outsideTemp[4];
   char insideTemp[4];
   char speed[3];
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

  tft.setRotation(1);
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
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);   // 5 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

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
  // Our supplier changed the 1.8" display slightly after Jan 10, 2012
  // so that the alignment of the TFT had to be shifted by a few pixels
  // this just means the init code is slightly different. Check the
  // color of the tab to see which init code to try. If the display is
  // cut off or has extra 'random' pixels on the top & left, try the
  // other option!
  // If you are seeing red and green color inversion, use Black Tab

  // If your TFT's plastic wrap has a Black Tab, use the following:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab

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
  delay(100);
}


void drawInitialUI()
{
  //Temp boxes
  tft.drawRect( 5,  5, 150, 15, fgColor);
  tft.fillRect( 5,  5,150/2,15, fgColor);
  
  tft.drawRect( 5, 48,  70,  9, fgColor);
  tft.fillRect( 5, 48,70/2,  9, fgColor);
  
  tft.drawRect(85, 48,  70,  9, fgColor);
  tft.fillRect(85, 48,70/2,  9, fgColor);

  //Box for speed
  //tft.drawRect( 5, 88,  75, 35, fgColor);
  
  uint16_t time = millis();
  tftdrawNumber(8, 5, 88);
  time = millis() - time;
  Serial.print("Draw 8 time: ");
  Serial.println(time, DEC);
  
  time = millis();
  tftdrawNumber(9, 46, 88);
  time = millis() - time;
  Serial.print("Draw 9 time: ");
  Serial.println(time, DEC);
  
  
  //Strings
  tft.setCursor( 5, 25);
  tft.print("ENGINE COOLANT 285F"); //20*6 = 120 (rect is 150)

  tft.setCursor( 5, 64);
  tft.print("OUTSIDE 100F"); //13*6 = 78 (rect is 70)
  
  tft.setCursor(85, 64);
  tft.print("INSIDE 100F"); //12*6 = 72 (rect is 70)
  
  
}

void calculateUI()
{
  sprintf(displayF.coolantTemp, "%3d", 212);  
  sprintf(displayF.outsideTemp, "%3d", 32);  
  sprintf(displayF.insideTemp, "%3d", -40);  
 
  
  sprintf(displayC.coolantTemp, "%3d", 100);  
  sprintf(displayC.outsideTemp, "%3d", 0);  
  sprintf(displayC.insideTemp, "%3d", -40);  
  
  if (sensorsUpdated.speed != 0) {
    
    //Serial.print("Speed (knots): "); Serial.println(sensorValues.speed);
    //Serial.print("Speed   (mph): "); Serial.println(sensorValues.speed * 1.15078);
    //Serial.print("Speed   (kph): "); Serial.println(sensorValues.speed * 1.852);

    //TODO: support 100+ numbers
    sprintf(displayF.speed, "%2d", (int)((sensorValues.speed * 1.15078) + 0.5)); //mph round up 
    sprintf(displayC.speed, "%2d", (int)((sensorValues.speed * 1.852) + 0.5)); //kph round up 
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

void updateUI()
{
  if (sensorsUpdated.speed != 0) {  
    //draw text
    Serial.println(displayF.speed);
    Serial.println(displayC.speed);

    tft.setCursor(5, 77);
    tft.print(displayF.speed);
    
    
    tftdrawNumber(displayF.speed[0],  5, 88);
    tftdrawNumber(displayF.speed[1], 46, 88);
    
    
    sensorsUpdated.speed = 0;    
  }
  
  
  if (sensorsUpdated.min != 0) {  
    //draw text
    Serial.println(displayF.time);
    Serial.println(displayC.time);

    tft.setCursor(107, 100);
    tft.print(displayF.time);
    
    sensorsUpdated.hour = 0;
    sensorsUpdated.min = 0;    
  }
  
  if (sensorsUpdated.day != 0) {  
    //draw text
    Serial.println(displayF.date);
    Serial.println(displayC.date);

    tft.setCursor(107, 115);
    tft.print(displayF.date);

    sensorsUpdated.month = 0;
    sensorsUpdated.day = 0;
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
  if (millis() - timer > 500) { //2000) { 
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
    */
    
    if (sensorValues.month != GPS.month || sensorValues.day != GPS.day)
    {
      sensorValues.month = GPS.month;
      sensorValues.day   = GPS.day;
      
      sensorsUpdated.month = 1;
      sensorsUpdated.day = 1;
    }

    //TODO: this is UTC time
    if (sensorValues.hour != GPS.hour || sensorValues.min != GPS.minute)
    {
      sensorValues.hour  = GPS.hour;
      sensorValues.min   = GPS.minute;
      
      sensorsUpdated.hour = 1;
      sensorsUpdated.min = 1;
    }
    
    //TODO: Just assume this is always updating?
    //TODO: Should at least calculate mph values to compare
    sensorValues.fix   = GPS.fix;
    if (GPS.fix){
      
      if (sensorValues.speed != GPS.speed)
      {
        sensorValues.speed = GPS.speed;
        sensorsUpdated.speed = 1;
      }
    }
    calculateUI();
    
    updateUI();
  }
}



void tftdrawNumber(char number, int x, int y)
{
  switch(number){
    case '0': 
      tftdraw0(x, y);
    break; 
    case '1': 
      tftdraw1(x, y);
    break; 
    case '2': 
      tftdraw2(x, y);
    break; 
    case '3': 
      tftdraw3(x, y);
    break; 
    case '4': 
      tftdraw4(x, y);
    break; 
    case '5': 
      tftdraw5(x, y);
    break; 
    case '6': 
      tftdraw6(x, y);
    break; 
    case '7': 
      tftdraw7(x, y);
    break; 
    case '8': 
      tftdraw8(x, y);
    break; 
    case '9': 
      tftdraw9(x, y);
    break; 
    default:
      tftdrawBlank(x, y);
    break;
  }
}


//32*35 chars
static unsigned char PROGMEM icon32_num0[] =
{ B01111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B01111111, B11111111, B11111111, B11111111
  };

static unsigned char PROGMEM icon32_num1[] =
{ B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111
  };

static unsigned char PROGMEM icon32_num2[] =
{ B01111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B01111111, B11111111, B11111111, B11111111
  };

static unsigned char PROGMEM icon32_num3[] =
{ B01111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B01111111, B11111111, B11111111, B11111111
  };

static unsigned char PROGMEM icon32_num4[] =
{ B01111111, B00000000, B00000000, B11111111,
  B11111111, B00000000, B00000000, B11111111,
  B11111111, B00000000, B00000000, B11111111,
  B11111111, B00000000, B00000000, B11111111,
  B11111111, B00000000, B00000000, B11111111,
  B11111111, B00000000, B00000000, B11111111,
  B11111111, B00000000, B00000000, B11111111,
  B11111111, B00000000, B00000000, B11111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111
  };

static unsigned char PROGMEM icon32_num5[] =
{ B01111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B01111111, B11111111, B11111111, B11111111
  };


static unsigned char PROGMEM icon32_num6[] =
{ B01111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B01111111, B11111111, B11111111, B11111111
  };

static unsigned char PROGMEM icon32_num7[] =
{ B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111
  };

static unsigned char PROGMEM icon32_num8[] =
{ B01111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B01111111, B11111111, B11111111, B11111111
  };

static unsigned char PROGMEM icon32_num9[] =
{ B01111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B00000000, B00000000, B01111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111,
  B00000000, B00000000, B00000000, B01111111
  };
void tftdrawBlank(int x, int y)
{
  //tftdraw0( x, y)
  tft.fillRect( x, y, 32, 35, bgColor);
}
void tftdraw0(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num0, 32, 35, fgColor, bgColor);
}
void tftdraw1(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num1, 32, 35, fgColor, bgColor);
}
void tftdraw2(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num2, 32, 35, fgColor, bgColor);
}
void tftdraw3(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num3, 32, 35, fgColor, bgColor);
}
void tftdraw4(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num4, 32, 35, fgColor, bgColor);
}
void tftdraw5(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num5, 32, 35, fgColor, bgColor);
}
void tftdraw6(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num6, 32, 35, fgColor, bgColor);
}
void tftdraw7(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num7, 32, 35, fgColor, bgColor);
}
void tftdraw8(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num8, 32, 35, fgColor, bgColor);
}
void tftdraw9(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num9, 32, 35, fgColor, bgColor);
}

