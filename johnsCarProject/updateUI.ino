


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


  for (;;);

}


void drawInitialUI()
{
  //Horiz lines
  tft.fillRect( 10, 135, 300, 3, fgColor);
  tft.fillRect( 10,  48, 300, 3, fgColor);

  //Engine temp Bar
  tft.drawRect( 0,  0,  320, 40, fgColor);
  tft.fillRect( 0,  0, 320 / 2, 40, fgColor);

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

  //
  tftdrawColon(258, 170);

}

void updateUI()
{
  static char oldSpeed0 = ' ';
  static char oldSpeed1 = ' ';
  static int displaySnowFlake = 0;
  
  int oldFgColor = fgColor;

  if (sensorsUpdated.coolantValue != 0) {
    drawUICoolantTemp();
    sensorsUpdated.coolantValue = 0;
  }
  if (sensorsUpdated.outsideValue != 0) {
    
    if (displayF.outsideTempValue <= 0) {
      fgColor = COLOR_BLUE;
    }
    
    tftdrawTextSmall(40, 65, displayF.outsideTemp[0]);
    tftdrawTextSmall(60, 65, displayF.outsideTemp[1]);
    tftdrawTextSmall(80, 65, displayF.outsideTemp[2]);
    //drawUIOutsideTemp();
    fgColor = oldFgColor;

    //Draw or delete snowflake
    if (displayF.outsideTempValue <= 32)
    {
      if (displaySnowFlake == 0) {
        tftdrawSnowflake(144, 65);
        displaySnowFlake = 1;
      }
    } else {
      if (displaySnowFlake != 0) {
        tft.fillRect( 144,  65, 30, 30, bgColor);
        displaySnowFlake = 0;
      }
    } //end draw snowflake


    sensorsUpdated.outsideValue = 0;
  }
  if (sensorsUpdated.insideValue != 0) {
    
    if (displayF.outsideTempValue <= 0) {
      fgColor = COLOR_BLUE;
    }

    tftdrawTextSmall(212, 65, displayF.insideTemp[0]);
    tftdrawTextSmall(232, 65, displayF.insideTemp[1]);
    tftdrawTextSmall(252, 65, displayF.insideTemp[2]);
    //drawUIInsideTemp();
    fgColor = oldFgColor;
    
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
    tftdrawTextSmall(214, 166, displayF.time[0]); //TODO: update john, 175
    tftdrawTextSmall(234, 166, displayF.time[1]); //TODO: update john, 175
    //mins
    tftdrawTextSmall(274, 166, displayF.time[3]); //TODO: update john, 175
    tftdrawTextSmall(294, 166, displayF.time[4]); //TODO: update john, 175

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


//HACK: lots of hack going on here
void drawUICoolantTemp()
{
  static int textLocation = 58;
  
  uint16_t barColor = fgColor;

  //save the bg and fg color, they will be changing temporarily
  uint16_t saveFgColor = fgColor;
  uint16_t saveBgColor = bgColor;

  //Color the bar accordingly
  if (displayF.coolantTempValue >= 251) {
    barColor = COLOR_RED;
  } else if (displayF.coolantTempValue >= 221) {
    barColor = COLOR_YELLOW;
  } else if (displayF.coolantTempValue <= 150) {
    barColor = COLOR_BLUE;
  }
  //Map the value to the width of the bar
  int width = map(displayF.coolantTempValue, 105, 260, 0, 318);
  //Truncate the ends to min/max
  if (width <= 10) width =  10;
  if (width > 318) width = 318;
  
  //Position the text to the left if 165 or over, to the right if under.
  //Force update bar drawing if changes
  if(displayF.coolantTempValue < 165 && textLocation != 250) //TODO: update john, 250 alternate
  {
    textLocation = 250;
    drawUITopBar(width, barColor, true);
  } else 
  if(displayF.coolantTempValue >= 165 && textLocation != 58)
  {
    textLocation = 58;
    drawUITopBar(width, barColor, true);
  } else {
    drawUITopBar(width, barColor, false);
  }

  //Make sure the text is the correct color based on placement and bar color
  switch(barColor) {
    case COLOR_YELLOW:
      fgColor = bgColor;
      bgColor = barColor;
    break;
    case COLOR_RED:
      bgColor = barColor;
    break;
    case COLOR_WHITE:
      if(textLocation == 58)
      {
        fgColor = bgColor;
        bgColor = barColor;
      }
    break;
  }

  tftdrawCoolant(7, 1);

  tftdrawTextSmall(textLocation +  0,  7, displayF.coolantTemp[0]); //TODO: update john, 5
  tftdrawTextSmall(textLocation + 20,  7, displayF.coolantTemp[1]); //TODO: update john, 5
  tftdrawTextSmall(textLocation + 40,  7, displayF.coolantTemp[2]); //TODO: update john, 5

  //Pop the saved fg and bg colors back
  fgColor = saveFgColor;
  bgColor = saveBgColor;


}
void drawUIOutsideTemp()
{
  uint16_t barColor = fgColor;

  if (displayF.outsideTempValue >= 100) {
    barColor = COLOR_RED;
  } else if (displayF.outsideTempValue >= 90) {
    barColor = COLOR_YELLOW;
  } else if (displayF.outsideTempValue <= 32) {
    barColor = COLOR_BLUE;
  }
  int width = map(displayF.outsideTempValue, 32 - 5, 105, 0, 68);
  if (width <= 5) width =  5;
  if (width > 68) width = 68;
  drawUILeftBar(width, barColor);
  if (displayF.outsideTempValue <= 32) {
    tft.setCursor( 16, 49);
    tft.setTextColor(COLOR_WHITE, COLOR_WHITE);
    tft.print("*");
    tft.setTextColor(fgColor, bgColor);
  }
}

void drawUIInsideTemp()
{
  uint16_t barColor = fgColor;

  if (displayF.insideTempValue >= 100) {
    barColor = COLOR_RED;
  } else if (displayF.insideTempValue >= 90) {
    barColor = COLOR_YELLOW;
  } else if (displayF.insideTempValue <= 32) {
    barColor = COLOR_BLUE;
  }
  int width = map(displayF.insideTempValue, 32 - 5, 105, 0, 68);
  if (width <= 5) width =  5;
  if (width > 68) width = 68;
  drawUIRightBar(width, barColor);
  if (displayF.insideTempValue <= 32) {
    tft.setCursor( 96, 49);
    tft.setTextColor(COLOR_WHITE, COLOR_WHITE);
    tft.print("*");
    tft.setTextColor(fgColor, bgColor);
  }
}

void drawUITopBar(int width, uint16_t barColor)
{
  drawUITopBar(width, barColor, false);
}

void drawUITopBar(int width, uint16_t barColor, boolean forceUpdate)
{
  static int lastWidth = -1;
  static uint16_t lastBarColor = -1;
  
  //If the color changes we have to redraw all of it.
  if (lastBarColor != barColor) {
    lastWidth = -1; //Redraw all 
  }
  
  if (forceUpdate) {
    lastWidth = -1; //Redraw all
  }
  //Dont need to draw anything if width doesnt change
  if (lastWidth == width) return;
  
  //Check if we need to redraw all
  if (lastWidth == -1) {
    tft.fillRect( 1, 1, width, 38, barColor);
    tft.fillRect( 1 + width, 1, 318 - width, 38, bgColor);
  //Else, only redraw what we add or remove
  } else {
    if (lastWidth < width) {    
      tft.fillRect( 1 + lastWidth, 1, width - lastWidth, 38, barColor);
    } else {
      tft.fillRect( 1 + width, 1, lastWidth - width, 38, bgColor);
    }
  }
  
  //Save width and color for next time
  lastWidth = width;
  lastBarColor = barColor;
}

void drawUILeftBar(int width, uint16_t barColor)
{
  static int lastWidth = 0;
  //fill min is 0-68
  tft.fillRect( 6, 49, width, 7, barColor);
  tft.fillRect( 6 + width, 49, 68 - width, 7, bgColor);
}

void drawUIRightBar(int width, uint16_t barColor)
{
  static int lastWidth = 0;
  //fill min is 0-68
  tft.fillRect( 86, 49, width, 7, barColor);
  tft.fillRect( 86 + width, 49, 68 - width, 7, bgColor);
}
