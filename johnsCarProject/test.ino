
void drawJohnsBar()
{
  int index;
  int val;
  //tft.fillRect(  10, 135, 300,  3, fgColor);
  //tft.fillRect(  10,  48, 300,  3, fgColor);
  
  //Remove Engine temp Bar
  //tft.drawRect(   0,   0, 320, 40, bgColor);

  uint16_t color = tft.Color565(255, 0, 0);
  
  
  tft.fillRect( 1, 1, 200,  1,   tft.Color565(155, 155, 0));
  tft.fillRect( 1, 2, 200,  1,   tft.Color565(170, 170, 0));
  tft.fillRect( 1, 3, 200,  1,   tft.Color565(195, 195, 0));
  tft.fillRect( 1, 4, 200,  1,   tft.Color565(215, 215, 0));
  
  tft.fillRect( 1, 5, 200, 38-4, tft.Color565(255, 255, 0));
  /**/


  //23 more
  //tft.fillRect( 1, 38, 200,  1,   tft.Color565(0, 255, 0));
  
  val = 215;
  for(index = 16; index <= 38; index++)
  {
    tft.fillRect( 1, index, 200,  1,   tft.Color565(val, val, 0));
    
    val -= 5;
  }
  
  //while(1);
}
