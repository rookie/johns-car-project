
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
  
  tftdrawCoolant(85, 64+8);
}

void updateUI()
{ 
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
  
  if (sensorsUpdated.speedKnots != 0) {  
    //draw text
    Serial.println(displayF.speed);
    Serial.println(displayC.speed);

    tft.setCursor(5, 77);
    tft.print(displayF.speed);
    
    //TODO: updating both numbers?
    tftdrawNumber(displayF.speed[0],  5, 88);
    tftdrawNumber(displayF.speed[1], 46, 88);
    
    sensorsUpdated.speedKnots = 0;    
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
