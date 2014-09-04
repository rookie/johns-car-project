


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

    
  //drawInitilUI();
  

  for(;;);
  
}


void drawInitialUI()
{
  //Horiz lines
  tft.fillRect( 10, 135,300, 3, fgColor);
  tft.fillRect( 10,  48,300, 3, fgColor);
  
  //Engine temp Bar
  tft.drawRect( 0,  0,  320, 40, fgColor);
  tft.fillRect( 0,  0,320/2, 40, fgColor);

  tftdrawCoolant( 1, 1);

  //Outside
  tftdrawTherm(   6,  57);
  tftdrawDegree(102,  65);
  tftdrawF(120,  65);
  tftdrawOutside(41, 105);

  //Snowflake
  //tftdrawSnowflake(144, 65);
  
  //Inside 
  tftdrawTherm( 179,  57);
  tftdrawDegree(274,  65);
  tftdrawF(292,  65);
  tftdrawInside(219, 105);

  //MPH
  tftdrawMph(177, 150);
    
  //Time/date box
  tft.drawRect( 208, 157, 107, 66, fgColor);
  tft.drawRect( 209, 158, 105, 64, fgColor);
  
}

void updateUI()
{ 
  static char oldSpeed0 = ' ';
  static char oldSpeed1 = ' ';
  
  
  if(sensorsUpdated.coolantValue != 0){
    //drawUICoolantTemp();
    sensorsUpdated.coolantValue = 0;
  }
  if(sensorsUpdated.outsideValue != 0){
    tftdrawTextSmall(40, 65, displayF.outsideTemp[0]);
    tftdrawTextSmall(60, 65, displayF.outsideTemp[1]);
    tftdrawTextSmall(80, 65, displayF.outsideTemp[2]);
    //drawUIOutsideTemp();
    sensorsUpdated.outsideValue = 0;
  }
  if(sensorsUpdated.insideValue != 0){
    tftdrawTextSmall(212, 65, displayF.insideTemp[0]);
    tftdrawTextSmall(232, 65, displayF.insideTemp[1]);
    tftdrawTextSmall(252, 65, displayF.insideTemp[2]);
    //drawUIInsideTemp();
    sensorsUpdated.insideValue = 0;
  }
  
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
  
  
  if (sensorsUpdated.min != 0) {  
    //draw text
    Serial.println(displayF.time);
    Serial.println(displayC.time);
    
    //hours
    tftdrawTextSmall(214, 166, displayF.time[0]); //166 was 175
    tftdrawTextSmall(234, 166, displayF.time[1]);
    //mins
    tftdrawTextSmall(274, 166, displayF.time[3]);
    tftdrawTextSmall(294, 166, displayF.time[4]);
    
    sensorsUpdated.hour = 0;
    sensorsUpdated.min = 0;    
  }
  
  if (sensorsUpdated.day != 0) {  
    //draw text
    Serial.println(displayF.date);
    Serial.println(displayC.date);

    tft.setTextSize(2);
    tft.setCursor(230, 200);
    tft.print(displayF.date);
    tft.setTextSize(1);

    /*
    //month
    tftdrawTextSmall(214, 203, displayF.date[0]);
    tftdrawTextSmall(234, 203, displayF.date[1]);
    //day
    tftdrawTextSmall(274, 203, displayF.date[3]);
    tftdrawTextSmall(294, 203, displayF.date[4]);
    */
    

    sensorsUpdated.month = 0;
    sensorsUpdated.day = 0;
  }
  
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
