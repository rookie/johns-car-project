
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

#define speedFontM 0x00001000
#define speedFontN 0x00002000
#define speedFontO 0x00004000
#define speedFontP 0x00008000


void tftdrawSeven(int x, int y, int a, int b, int c, int d, int e, int f, int g)
{
  //Do yo math
  if(a) tft.fillRect(    x,    y, 80, 21, fgColor);
  if(b) tft.fillRect( x+62, y+21, 18, 17, fgColor);
  if(c) tft.fillRect( x+62, y+54, 18, 17, fgColor);
  if(d) tft.fillRect(    x, y+71, 80, 21, fgColor);
  if(e) tft.fillRect(    x, y+54, 16, 17, fgColor);
  if(f) tft.fillRect(    x, y+21, 16, 17, fgColor);
  if(g) tft.fillRect(    x, y+38, 80, 16, fgColor);
}


void tftdrawBlank(int x, int y)
{
  tft.fillRect( x, y, 80, 92, bgColor);
}

void tftdraw0(int x, int y)
{
  tftdrawSeven(x, y, 1,1,1,1,1,1,0);
}

void tftdraw1(int x, int y)
{
  tft.fillRect( x+62, y, 18, 92, fgColor);
}

void tftdraw2(int x, int y)
{
  tftdrawSeven(x, y, 1,1,0,1,1,0,1);
}

void tftdraw3(int x, int y)
{
  tftdrawSeven(x, y, 1,1,1,1,0,0,1);
}

void tftdraw4(int x, int y)
{
  tftdrawSeven(x, y, 0,1,1,0,0,1,1);
}

void tftdraw5(int x, int y)
{
  tftdrawSeven(x, y, 1,0,1,1,0,1,1);
}

void tftdraw6(int x, int y)
{
  tftdrawSeven(x, y, 1,0,1,1,1,1,1);
}

void tftdraw7(int x, int y)
{
  tftdrawSeven(x, y, 1,1,1,0,0,0,0);
}

void tftdraw8(int x, int y)
{
  tftdrawSeven(x, y, 1,1,1,1,1,1,1);
}

void tftdraw9(int x, int y)
{
  tftdrawSeven(x, y, 1,1,1,1,0,1,1);
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


void tftdrawNumber(char number, int x, int y)
{  
  tftdrawBlank(x, y);
    
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
      tftdraw1(x, y);
      tftdraw4(x, y);
    break; 
    case '5': 
      tftdraw5(x, y);
    break; 
    case '6': 
      tftdraw6(x, y);
    break; 
    case '7': 
      tftdraw1(x, y);
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


