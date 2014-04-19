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

//TEST: testing drawBitmap function
static unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

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
  Serial.print("hello!");

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
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
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
}


void loop() {
  gpsLoop();
  delay(500);
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
  tftdraw8(5, 88);
  tftdraw9(46, 88);
  
  
  //Strings
  tft.setCursor( 5, 25);
  tft.print("ENGINE COOLANT 285ºF"); //20*6 = 120 (rect is 150)

  tft.setCursor( 5, 64);
  tft.print("OUTSIDE 100ºF"); //13*6 = 78 (rect is 70)
  
  tft.setCursor(85, 64);
  tft.print("INSIDE 100ºF"); //12*6 = 72 (rect is 70)
  
  
  //Icon Test
  tft.drawBitmap(100, 90,  logo16_glcd_bmp, 16, 16, fgColor);
  
}

void calculateUI()
{
  sprintf(displayF.coolantTemp, "%3d", 212);  
  sprintf(displayF.outsideTemp, "%3d", 32);  
  sprintf(displayF.insideTemp, "%3d", -40);  
 
  sprintf(displayF.speed, "%2d", (sensorValues.speed * 1.15078) + 0.5); //mph round up 
  
  sprintf(displayC.coolantTemp, "%3d", 100);  
  sprintf(displayC.outsideTemp, "%3d", 0);  
  sprintf(displayC.insideTemp, "%3d", -40);  
  
  sprintf(displayC.speed, "%2d", (sensorValues.speed * 1.852) + 0.5); //kph round up 

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
  if (sensorsUpdated.min != 0) {  
    //draw text
    Serial.println(displayF.time);
    Serial.println(displayC.time);

    sensorsUpdated.hour = 0;
    sensorsUpdated.min = 0;    
  }
  
  if (sensorsUpdated.day != 0) {  
    //draw text
    Serial.println(displayF.date);
    Serial.println(displayC.date);

    sensorsUpdated.month = 0;
    sensorsUpdated.day = 0;
  }
  
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST7735_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST7735_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST7735_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST7735_WHITE);
  tft.print(" seconds.");
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

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer
    
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
      sensorValues.speed = GPS.speed;
    }
    calculateUI();
    
    updateUI();
  }
}

//72*35 chars
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
  
//TODO: John doesnt have the modified libraries yet
void tftdraw8(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num8, 32, 35, fgColor);//, bgColor);
}
void tftdraw9(int x, int y)
{
  tft.drawBitmap(x, y,  icon32_num9, 32, 35, fgColor);//, bgColor);
}

