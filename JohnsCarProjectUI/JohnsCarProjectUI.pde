/**
 * JohnsCarProjectUI
 * by Matt Hurley
 * 
 * UI Mock up for johns car project
 */
 
  int bgColor;
  int fgColor;

  float textSize = 12;
  float cursorX = 0;
  float cursorY = 0;

void setup()
{
  size(160, 128);
  bgColor = 0;
  fgColor = 255;
  
  textSize(textSize);

  background(bgColor);
  drawInitialUI();
}

void drawInitialUI()
{
  //Temp boxes
  tftdrawRect( 5,  5, 150, 15, fgColor);
  tftfillRect( 5,  5,150/2,15, fgColor);
  
  tftdrawRect( 5, 48,  70,  9, fgColor);
  tftfillRect( 5, 48,70/2,  9, fgColor);
  
  tftdrawRect(85, 48,  70,  9, fgColor);
  tftfillRect(85, 48,70/2,  9, fgColor);

  //Box for speed
  //tftdrawRect( 5, 88,  75, 35, fgColor);
  
  //Strings
  tftsetCursor( 5, 25);
  tftprint("ENGINE COOLANT 285ºF"); //20*6 = 120 (rect is 150)

  tftsetCursor( 5, 64);
  tftprint("OUTSIDE 100ºF"); //13*6 = 78 (rect is 70)
  
  tftsetCursor(85, 64);
  tftprint("INSIDE 100ºF"); //12*6 = 72 (rect is 70)
  
  //MPH/KPH
  tftsetCursor(87, 88);
  tftprint("M");
  tftsetCursor(87, 101);
  tftprint("P");
  tftsetCursor(87, 115);
  tftprint("H");
  
  drawTime();
  drawDate();
  
  tftdraw8(0, 0);
  tftdraw9(0, 0);
  
  /*
  //Icon Test
  tft.drawBitmap(100, 90,  logo16_glcd_bmp, 16, 16, fgColor);
  */
  
  //save("gaugev1.bmp");
}
 
void draw()
{
  
}

void drawTime()
{
  tftsetCursor(107, 100);
  tftprint("12:34");
}
void drawDate()
{ 
  tftsetCursor(107, 115);
  tftprint("12/31");
}


//Helpers

void tftprint(String str)
{
  //cursorY+3 is the top line
  //line(0, cursorY, 200, cursorY);
  //line(0, cursorY+textSize, 200, cursorY+textSize);
  text(str, cursorX, cursorY + textSize - 3); //3 to align top (12pt)
}

void tftsetCursor(float x, float y)
{
  cursorX = x;
  cursorY = y;
}

void tftfillRect(float x, float y, float w, float h, color c)
{
  stroke(fgColor);
  fill(fgColor);
  rect(x, y, w, h);
}


void tftdrawRect(float x, float y, float w, float h, color c)
{
  stroke(fgColor);
  noFill();
  rect(x, y, w, h);
}
void tftdraw7(float x, float y)
{
  x = 5;
  y = 88;
  float w = 32; //or 40?
  float h = 35;
  //draw main background
  noStroke();
  fill(fgColor);
  rect(x, y, w, h);
  //remove left bottom
  fill(bgColor);
  rect(x, y+8, w-7, h-8);
}
void tftdraw8(float x, float y)
{
  //width is 
  x = 5;
  y = 88;
  float w = 32; //or 40?
  float h = 35;
  //draw main background
  noStroke();
  fill(fgColor);
  rect(x, y, w, h);
  //remove center top
  fill(bgColor);
  rect(x+8, y+8, w-7-8, 6);
  //remove center bottom
  rect(x+8, y+8+6+7, w-7-8, 6);
  //remove corners
  point(x,y);
  point(x,y+h-1);
}
void tftdraw9(float x, float y)
{
  x = 46;
  y = 88;
  float w = 32; //or 40?
  float h = 35;
  //draw main background
  noStroke();
  fill(fgColor);
  rect(x, y, w, h); 
  //remove center top
  fill(bgColor);
  rect(x+8, y+8, w-7-8, 6);
  //remove left bottom
  rect(x, y+8+6+7, w-7, 6+8);
}
