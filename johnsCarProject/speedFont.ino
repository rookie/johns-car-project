

/********************


Current
+--------------------+
|                    |
|         A          |
+--------------------+
|   |            |   |
| F |            | B |
+--------------------+
|                    |
|         G          |
+--------------------+
|   |            |   |
| E |            | C |
+--------------------+
|                    |
|         D          |
+--------------------+


Proposed
+--------------------+
|   |            |   |
| A |     B      | C |
+--------------------+
|   |            |   |
| L |            | D |
+--------------------+
|   |            |   |
| K |     M      | E |
+--------------------+
|   |            |   |
| J |            | F |
+--------------------+
|   |            |   |
| I |     H      | G |
+--------------------+












*******************/









#define speedFontA 0x00000001
#define speedFontB 0x00000002
#define speedFontC 0x00000004
#define speedFontD 0x00000008

#define speedFontE 0x00000010
#define speedFontF 0x00000020
#define speedFontG 0x00000040
#define speedFontH 0x00000080

#define speedFontI 0x00000100
#define speedFontJ 0x00000200
#define speedFontK 0x00000400
#define speedFontL 0x00000800

//#define speedFontM 0x00001000
//#define speedFontN 0x00002000
//#define speedFontO 0x00004000
//#define speedFontP 0x00008000

#define number0 (speedFontA | speedFontB | speedFontC | speedFontD | speedFontE | speedFontF | speedFontG | speedFontH | speedFontI | speedFontJ | speedFontK)
#define number1 (                          speedFontC | speedFontD | speedFontE | speedFontF | speedFontG                                                                 )
#define number2 (speedFontA | speedFontB | speedFontC | speedFontD | speedFontE |              speedFontG | speedFontH | speedFontI | speedFontJ |              speedFontL) //John: K segment was wrong
#define number3 (speedFontA | speedFontB | speedFontC | speedFontD | speedFontE | speedFontF | speedFontG | speedFontH |                                        speedFontL)
#define number4 (speedFontA |              speedFontC | speedFontD | speedFontE | speedFontF | speedFontG |                           speedFontJ | speedFontK | speedFontL)
#define number5 (speedFontA | speedFontB | speedFontC |              speedFontE | speedFontF | speedFontG | speedFontH |              speedFontJ | speedFontK | speedFontL)
#define number6 (speedFontA | speedFontB | speedFontC |              speedFontE | speedFontF | speedFontG | speedFontH | speedFontI | speedFontJ | speedFontK | speedFontL)
#define number7 (speedFontA | speedFontB | speedFontC | speedFontD | speedFontE | speedFontF | speedFontG                                                                 )
#define number8 (speedFontA | speedFontB | speedFontC | speedFontD | speedFontE | speedFontF | speedFontG | speedFontH | speedFontI | speedFontJ | speedFontK | speedFontL)
#define number9 (speedFontA | speedFontB | speedFontC | speedFontD | speedFontE | speedFontF | speedFontG | speedFontH |              speedFontJ | speedFontK | speedFontL)


void drawSevenDiff(int x, int y, int fromOld, int toNew)
{
  int same = fromOld & toNew;
  int deleted = fromOld ^ same;
  int added = toNew ^ same;
  
  //delete old ones
  drawSeven(deleted, x, y, bgColor);
  
  //add new ones
  drawSeven(added, x, y, fgColor);
  
  
}

void drawSeven(int segments, int x, int y, uint16_t color)
{
  if(segments & speedFontA) tft.fillRect(    x,    y, 16, 21, color);
  if(segments & speedFontB) tft.fillRect( x+16,    y, 46, 21, color);
  if(segments & speedFontC) tft.fillRect( x+62,    y, 18, 21, color);
  if(segments & speedFontD) tft.fillRect( x+62, y+21, 18, 17, color);
  if(segments & speedFontE) tft.fillRect( x+62, y+38, 18, 16, color);
  if(segments & speedFontF) tft.fillRect( x+62, y+54, 18, 17, color);
  if(segments & speedFontG) tft.fillRect( x+62, y+71, 18, 21, color);
  
  if(segments & speedFontH) tft.fillRect(    x, y+71, 62, 21, color);
  if(segments & speedFontI) tft.fillRect(    x, y+54, 16, 17, color);
  if(segments & speedFontJ) tft.fillRect(    x, y+38, 16, 16, color);
  if(segments & speedFontK) tft.fillRect(    x, y+21, 16, 17, color);
  if(segments & speedFontL) tft.fillRect( x+16, y+38, 46, 16, color);
  
  //Dashes
  if(segments & speedFontD) tft.fillRect( x+62, y+21+15, 4, 2, bgColor);
  if(segments & speedFontF) tft.fillRect( x+62, y+54, 4, 2, bgColor);
  
  if(segments & speedFontI) tft.fillRect(    x+12, y+54, 4, 2, bgColor);
  if(segments & speedFontK) tft.fillRect(    x+12, y+21+15, 4, 2, bgColor);
  
}

void tftdrawBlank(int x, int y)
{
  tft.fillRect( x, y, 80, 92, bgColor);
}

/*
void printthings()
{
  const uint8_t *bitmap = speed_num2;
  int16_t w = 80;
  int16_t h = 92;

  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
	Serial.print("1");
      } else {
        Serial.print("0");
      }
    }
    Serial.println(",");
  }
}
*/

int segmentMaskForNumber(char number)
{
  int mask = 0;
  switch(number){
    case '0': 
      mask = number0;
    break; 
    case '1': 
      mask = number1;
    break; 
    case '2': 
      mask = number2;
    break; 
    case '3': 
      mask = number3;
    break; 
    case '4': 
      mask = number4;
    break; 
    case '5': 
      mask = number5;
    break; 
    case '6': 
      mask = number6;
    break; 
    case '7': 
      mask = number7;
    break; 
    case '8': 
      mask = number8;
    break; 
    case '9': 
      mask = number9;
    break; 
    default:
      mask = 0;
    break;
  }
    
  return mask;
}


void tftdrawNumberFrom(char number, char oldNumber, int x, int y)
{
  int oldNumberMask = segmentMaskForNumber(oldNumber);
  int numberMask = segmentMaskForNumber(number);
  
  if(oldNumberMask == 0)
  { 
      tftdrawBlank(x, y);
  }
  
  //Remove flag on previous 1,4
  if (oldNumber == '1'){
    tweak1(x, y, bgColor);
  }
  if (oldNumber == '4'){
    tweak4(x, y, bgColor);
  }
  
  drawSevenDiff(x, y, oldNumberMask, numberMask);
   
  //Add flag on 1,4
  if (number == '1'){
    tweak1(x, y, fgColor);
  } 
  if (number == '4'){
    tweak4(x, y, fgColor);
  }
    
}


void tweak1(int x, int y, int color)
{
  tft.fillRect( x+59, y, 3, 21, color);
}

void tweak4(int x, int y, int color)
{
  tft.fillRect( x+16, y, 3, 21, color);
}

