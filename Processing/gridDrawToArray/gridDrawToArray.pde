


int gridWidth = 32; //not sure if it works without a multiple of 8
int gridHeight = 32;

//Speed numbers size
//int gridWidth = 32; 
//int gridHeight = 35;

int gridSize = 20;
int fgColor = 0;
int bgColor = 255;

void setup() {
  size(gridWidth*gridSize, gridHeight*gridSize);
  //smooth();
    
  background(bgColor);
  
  drawGrid();
  if(gridWidth == 32 && gridHeight == 35)
    loadArray(); 
}

void draw() {
  
  drawGrid();
}

void mousePressed(MouseEvent e)
{
  //convert mouseXY to gridXY
  int x = mouseX / gridSize;
  int y = mouseY / gridSize;
  
  int drawColor = bgColor;
  if (e.getButton() == LEFT) {
    drawColor = fgColor;
  } else if (e.getButton() == RIGHT) {
    drawColor = bgColor;  
  }
  
  drawPixel(x, y, drawColor);
}

void mouseDragged(MouseEvent e) 
{
  mousePressed(e);
}

void mouseReleased() {
  //printArray();
  printArrayFromScreen();
}

boolean isOn(int x, int y){
  boolean isOn = false; //off
  //check center to see if on at all
  color cp = get(x*gridSize+gridSize/2, y*gridSize+gridSize/2);
  if (cp == color(fgColor)) {
    isOn = true;
  } else {
    isOn = false;
  }
  
  return isOn;
}

void drawPixel(int x, int y, int fillColor) {
  noStroke();
  //clear old one
  fill(fillColor);
  rect(x*gridSize, y*gridSize, gridSize, gridSize);
}

void loadArray()
{
  int x = 0;
  int y = 0;
  
  for(x = 0; x < gridWidth/8; x++)
  {
    for(y = 0; y < gridHeight; y++)
    { 
      if((bitmap[x+y*gridWidth/8] & B10000000) != 0)
        drawPixel(x*8, y, fgColor);
      if((bitmap[x+y*gridWidth/8] & B01000000) != 0)
        drawPixel(x*8+1, y, fgColor);
      if((bitmap[x+y*gridWidth/8] & B00100000) != 0)
        drawPixel(x*8+2, y, fgColor);
      if((bitmap[x+y*gridWidth/8] & B00010000) != 0)
        drawPixel(x*8+3, y, fgColor);
      if((bitmap[x+y*gridWidth/8] & B00001000) != 0)
        drawPixel(x*8+4, y, fgColor);
      if((bitmap[x+y*gridWidth/8] & B00000100) != 0)
        drawPixel(x*8+5, y, fgColor);
      if((bitmap[x+y*gridWidth/8] & B00000010) != 0)
        drawPixel(x*8+6, y, fgColor);
      if((bitmap[x+y*gridWidth/8] & B00000001) != 0)
        drawPixel(x*8+7, y, fgColor);
    }
  }
}

void updateArray(int x, int y, int stateOn)
{
  if(stateOn != 0)
  {
    
  } else {
    
  }
}

void printArrayFromScreen()
{
  
  int x = 0;
  int y = 0;
   
  for(y = 0; y < gridHeight; y++)
  { 
    for(x = 0; x < gridWidth/8; x++)
    {
      int nextByteValue = 0;
      if(isOn(x*8, y))
        nextByteValue += B10000000;
      if(isOn(x*8+1, y))
        nextByteValue += B01000000;
      if(isOn(x*8+2, y))
        nextByteValue += B00100000;
      if(isOn(x*8+3, y))
        nextByteValue += B00010000;
      if(isOn(x*8+4, y))
        nextByteValue += B00001000;
      if(isOn(x*8+5, y))
        nextByteValue += B00000100;
      if(isOn(x*8+6, y))
        nextByteValue += B00000010;
      if(isOn(x*8+7, y))
        nextByteValue += B00000001;
      print("B");
      print(binary(nextByteValue, 8));
      if(x == gridWidth/8 - 1){
        println(",");
      } else {
        print(", ");
      }
    }
  }
}

void printArray()
{
  int x = 0;
  int y = 0;
   
  for(y = 0; y < gridHeight; y++)
  { 
    for(x = 0; x < gridWidth/8; x++)
    {
      int nextByteValue = 0;
      if((bitmap[x+y*gridWidth/8] & B10000000) != 0)
        nextByteValue += B10000000;
      if((bitmap[x+y*gridWidth/8] & B01000000) != 0)
        nextByteValue += B01000000;
      if((bitmap[x+y*gridWidth/8] & B00100000) != 0)
        nextByteValue += B00100000;
      if((bitmap[x+y*gridWidth/8] & B00010000) != 0)
        nextByteValue += B00010000;
      if((bitmap[x+y*gridWidth/8] & B00001000) != 0)
        nextByteValue += B00001000;
      if((bitmap[x+y*gridWidth/8] & B00000100) != 0)
        nextByteValue += B00000100;
      if((bitmap[x+y*gridWidth/8] & B00000010) != 0)
        nextByteValue += B00000010;
      if((bitmap[x+y*gridWidth/8] & B00000001) != 0)
        nextByteValue += B00000001;
      print("B");
      print(binary(nextByteValue, 8));
      if(x == gridWidth/8 - 1){
        println(",");
      } else {
        print(", ");
      }
    }
  }
}

void drawGrid(){
  int i = 0;
  int j = 0;
  
  stroke(0);
  strokeWeight(1);
  
  for(i = 0; i <= width; i+=gridSize)
  {
    for(j = 0; j <= height; j+=gridSize)
    {
      point(i, j);
    }
  }
}




int[] bitmap = 
//static unsigned char PROGMEM icon32_num2[] =
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
