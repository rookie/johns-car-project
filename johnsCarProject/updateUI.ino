


// Bounding boxes for debug
void drawDebugUI()
{
  
  //Horiz lines
  tft.fillRect(  10, 135, 300,  3, fgColor);
  tft.fillRect(  10,  48, 300,  3, fgColor);
  //Engine temp Bar
  tft.drawRect(   0,   0, 320, 40, fgColor);
  //Out Therm
  tft.drawRect(   6,  57,  32, 70, fgColor);
  //Out Num
  tft.drawRect(  40,  65,  16, 27, fgColor);
  tft.drawRect(  60,  65,  16, 27, fgColor);
  tft.drawRect(  80,  65,  16, 27, fgColor);
  //Out degree
  tft.drawRect( 102,  65,  12, 12, fgColor);
  //Out F
  tft.drawRect( 120,  65,  16, 27, fgColor);
  //Out Text
  tft.drawRect(  41, 105, 123, 16, fgColor);

  //Snowflake
  tft.drawRect( 144,  65,  30, 30, fgColor);
  
  //In Therm
  tft.drawRect( 179,  57,  32, 70, fgColor); //TODO: update john, 179
  //In Num
  tft.drawRect( 212,  65,  16, 27, fgColor); //TODO: update john, 65
  tft.drawRect( 232,  65,  16, 27, fgColor); //TODO: update john, 65
  tft.drawRect( 252,  65,  16, 27, fgColor); //TODO: update john, 65, 252
  //In degree
  tft.drawRect( 274,  65,  12, 12, fgColor);
  //In F
  tft.drawRect( 292,  65,  16, 27, fgColor);
  //In Text
  tft.drawRect( 219, 105,  89, 16, fgColor);
  

  //Speed
  tft.drawRect( 0, 148,  80, 92, fgColor);
  tft.drawRect( 89, 148,  80, 92, fgColor);

  //MPH
  tft.drawRect( 177, 150, 29, 89, fgColor);

  //GPS stuff  

  for(;;);
  
}


void drawInitialUI()
{
  /*
  //Horiz lines
  tft.fillRect( 10, 135,300, 3, fgColor);
  tft.fillRect( 10,  48,300, 3, fgColor);
  
  //Engine temp Bar
  tft.drawRect( 0,  0,  320, 40, fgColor);
  tft.fillRect( 0,  0,320/2, 40, fgColor);





  //Outside temp therm
  tft.drawRect(   6, 57, 32, 70, fgColor);
  
  
  
  //Inside temp therm
  
  
  
  
  //Out Therm
  //In Therm
  tft.drawRect( 182, 57 , 32, 70, fgColor);
  
  tft.setTextSize(2);
  //Strings
  tft.setCursor( 5, 25);
  //tft.print("ENGINE COOLANT 285F"); //20*6 = 120 (rect is 150)

  tft.setCursor( 41, 105);
  tft.print("OUTSIDE"); //13*6 = 78 (rect is 70)
  
  tft.setCursor( 219, 105);
  tft.print("INSIDE"); //12*6 = 72 (rect is 70)
  
  //MPH
  //
  
  //Speed
  //tft.drawRect( 5, 88,  75, 35, fgColor);
    */
    
    
  
}

void updateUI()
{ 
  static char oldSpeed0 = ' ';
  static char oldSpeed1 = ' ';
  
  /*
  if(sensorsUpdated.coolantValue != 0){
    tft.setCursor( 5+15*6, 25);
    tft.print(displayF.coolantTemp);
    drawUICoolantTemp();
    sensorsUpdated.coolantValue = 0;
  }
  if(sensorsUpdated.outsideValue != 0){
    tft.setCursor( 5+8*6, 64);
    tft.print(displayF.outsideTemp);
    drawUIOutsideTemp();
    sensorsUpdated.outsideValue = 0;
  }
  if(sensorsUpdated.insideValue != 0){
    tft.setCursor(85+7*6, 64);
    tft.print(displayF.insideTemp);
    drawUIInsideTemp();
    sensorsUpdated.insideValue = 0;
  }
  */
  
  if (sensorsUpdated.speedKnots != 0) {  
    //draw text
    Serial.println(displayF.speed);
    Serial.println(displayC.speed);

    //DEBUG
    tft.setCursor(170, 140);
    //tft.print(displayF.speed);
        
    tftdrawNumberFrom(displayF.speed[0], oldSpeed0,  0, 148);
    tftdrawNumberFrom(displayF.speed[1], oldSpeed1, 89, 148);
    
    oldSpeed0 = displayF.speed[0];
    oldSpeed1 = displayF.speed[1];
    
    sensorsUpdated.speedKnots = 0;    
  }
  
  /*
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
  */
}



void drawUICoolantTemp()
{
  uint16_t barColor = fgColor;
  
  if(displayF.coolantTempValue >= 251) {
    barColor = COLOR_RED;
  } else if(displayF.coolantTempValue >= 221) {
    barColor = COLOR_YELLOW;
  } else if(displayF.coolantTempValue <= 150){
    barColor = COLOR_BLUE;
  }
  int width = map(displayF.coolantTempValue, 105, 260, 0, 148);
  if(width <= 10) width =  10;
  if(width > 148) width = 148;
  drawUITopBar(width, barColor);
}
void drawUIOutsideTemp()
{
  uint16_t barColor = fgColor;
  
  if(displayF.outsideTempValue >= 100) {
    barColor = COLOR_RED;
  } else if(displayF.outsideTempValue >= 90) {
    barColor = COLOR_YELLOW;
  } else if(displayF.outsideTempValue <= 32){
    barColor = COLOR_BLUE;
  }
  int width = map(displayF.outsideTempValue, 32-5, 105, 0, 68);
  if(width <= 5) width =  5;
  if(width > 68) width = 68;
  drawUILeftBar(width, barColor);
  if(displayF.outsideTempValue <= 32) {
    tft.setCursor( 16, 49);
    tft.setTextColor(COLOR_WHITE, COLOR_WHITE);
    tft.print("*");
    tft.setTextColor(fgColor, bgColor);
  }
}

void drawUIInsideTemp()
{
  uint16_t barColor = fgColor;
  
  if(displayF.insideTempValue >= 100) {
    barColor = COLOR_RED;
  } else if(displayF.insideTempValue >= 90) {
    barColor = COLOR_YELLOW;
  } else if(displayF.insideTempValue <= 32){
    barColor = COLOR_BLUE;
  }
  int width = map(displayF.insideTempValue, 32-5, 105, 0, 68);
  if(width <= 5) width =  5;
  if(width > 68) width = 68;
  drawUIRightBar(width, barColor);
  if(displayF.insideTempValue <= 32) {
    tft.setCursor( 96, 49);
    tft.setTextColor(COLOR_WHITE, COLOR_WHITE);
    tft.print("*");
    tft.setTextColor(fgColor, bgColor);
  }
}

void drawUITopBar(int width, uint16_t barColor)
{
  static int lastWidth = 0;
  //fill min is 0-148
  tft.fillRect( 6, 6, width, 13, barColor);
  tft.fillRect( 6+width, 6, 148-width, 13, bgColor);
}

void drawUILeftBar(int width, uint16_t barColor)
{
  static int lastWidth = 0;
  //fill min is 0-68
  tft.fillRect( 6, 49, width, 7, barColor);
  tft.fillRect( 6+width, 49, 68-width, 7, bgColor);
}

void drawUIRightBar(int width, uint16_t barColor)
{
  static int lastWidth = 0;
  //fill min is 0-68
  tft.fillRect( 86, 49, width, 7, barColor);
  tft.fillRect( 86+width, 49, 68-width, 7, bgColor);
}
