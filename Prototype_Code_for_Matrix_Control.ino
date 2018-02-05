//Pin connected to ST_CP
int latchPin = 9;
//Pin connected to SH_CP
int clockPin = 12;
//Pin connected to DS
int dataPin = 11;
//Keeps track of the current states of the leds
boolean ledState[9][9] = {false};
boolean trueState[9][9] = {false};
boolean rowState[9] = {false};
boolean columnState[9] = {false};
//Stores whether the led matrix is on or not
boolean isOn = true;
//Stores the values to be passed to the shift registers
int reg[3] = {0};
//Variables used elsewhere that need to be initialized 
int ledToBlink[] = {0,0};
int data = 0;
int dataOut[] = {0,0};
int aniVar[] = {0,0};
int reading = HIGH;



void setup() {
  //Pin setup and serial started for the bluetooth module
  Serial1.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(6,INPUT);//used for power switch
  
}

void loop() {
  while(isOn)
  {
    //If data is coming in on the bluetooth module scan it
    if(Serial1.available()> 0)
    {
      scanBLE();
    }
    //Always need to blink leds, check if the switch is off, all lights are off that are supposed to be and then update the registers
    blinkAllLeds();
    checkIsOff();
    turnAllOffToo();
    lightLeds();
  }
  while(!isOn)
  {
    //Checks if the switch is turned on
    checkIsOn();
  }
  
}

//Checks and adjusts the state of the switch appropriately
void checkIsOff()
{
  reading = digitalRead(6);
  if(reading == LOW)
  {
    isOn = false;
  }
}
void checkIsOn()
{
  reading = digitalRead(6);
  if(reading == HIGH)
  {
    isOn = true;
  }
}

//Scans and executes the necessary action
void scanBLE()
{
  data = Serial1.read();
  alterTrueState(convertToLed(data));
}

//converts the incoming data to led value
int * convertToLed (int dataIn)
{
  int numChecking = dataIn;

  dataOut[0] = numChecking/10;
  dataOut[1] = numChecking%10;
  
  return dataOut;
}

//wave from side to side
void animation0()
{
  //scroll
  for (int x1 = 0; x1 < 9; x1++)
  {
    turnRowOn(x1);
    blinkFor(100);
    turnRowOff(x1);
  }
  delay(10);
  for (int x2 = 8; x2 >= 0; x2--)
  {
    turnRowOn(x2);
    blinkFor(100);
    turnRowOff(x2);
  }
  for (int y1 = 0; y1 < 9; y1++)
  {
    turnColOn(y1);
    blinkFor(100);
    turnColOff(y1);
  }
  delay(10);
  for (int y2 = 8; y2 >= 0; y2--)
  {
    turnColOn(y2);
    blinkFor(100);
    turnColOff(y2);
  }
  
}

//ripple from in to out then out to in
void animation1()
{
  for (int i = 4; i>=0;i--)
  {
    turnRingOn(i);
  }
  for (int j = 0; j<=4; j++)
  {
    turnRingOn(j);
  }
}
//used for turing on individual rings
void turnRingOn(int num)
{
  if(num!=4)
  {
    for(int x= 0; x <9;x++)
    {
      for(int y=0;y<9;y++)
      {
        if(x == num || y == num || x == 8-num || y == 8-num)
        {
          aniVar[0] = x;
          aniVar[1] = y;
          turnTrueStateOn(aniVar);
          if(x<num || y<num || x>8-num||y>8-num)
          {
            turnTrueStateOff(aniVar);
          }
        }
      }
    }

    blinkFor(100);
    turnAllOff();
  }
  else
  {
    aniVar[0] = 4;
    aniVar[1] = 4;
    turnTrueStateOn(aniVar);
    blinkFor(100);
    turnTrueStateOff(aniVar);
  }
}

//Checkerboard alternating
void animation2()
{
  for(int x =0; x < 9;x++)
  {
    for(int y = 0; y<9;y++)
    {
      if(x%2 == 0 && y%2 ==0)
      {
        aniVar[0] = x;
        aniVar[1] = y;
        turnTrueStateOn(aniVar);
      }
      if(x%2 == 1 && y%2 ==1)
      {
        aniVar[0] = x;
        aniVar[1] = y;
        turnTrueStateOn(aniVar);
      }
    }
  }
  blinkFor(100);
  for(int i=0;i<5;i++)
  {
    for(int x =0; x<9;x++)
    {
      for(int y=0; y<9;y++)
      {
        aniVar[0] = x;
        aniVar[1] = y;
        alterTrueState(aniVar);
      }
    }
    blinkFor(100);
  }
  turnAllOff();
}

//turns on current leds for an amount of time
void blinkFor(int t)
{
  for (int x = 0; x<t;x++)
  {
    blinkAllLeds();
  }
}

//blinks all leds at a rate so quick they look like they are all on at the same time
void blinkAllLeds()
{
  for (int x = 0; x < 9; x++)
  {
    for (int y = 0; y < 9; y++)
    {
      if(trueState[x][y] == true)
      {
        ledToBlink[0] = x;
        ledToBlink[1] = y;
        blinkLed(ledToBlink);
      }
    }
  }
}

//blinks an led once
void blinkLed(int led[])
{
  turnOn(led);
  lightLeds();
  turnOff(led);
}

//takes the values in the reg array and passes them to the shift registers to control the leds and adds a slight delay so it wont try to light multiple at once
void lightLeds()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, reg[2]);
  shiftOut(dataPin, clockPin, MSBFIRST, reg[1]);
  shiftOut(dataPin, clockPin, MSBFIRST, reg[0]);   
  digitalWrite(latchPin, HIGH);
  delay(.05);
}

//changes an led from on to off
void alterState (int led[])
{
  
  if (ledState[led[0]][led[1]])
  {
    turnOff(led);
  }
  else
  {
    turnOn(led);
  }
}
//Calls proper action due to input from serial
void alterTrueState (int led[])
{
  if (led[0] == 10)
  {
    turnAllOff();
    if(led[1] == 0)
    {
      animation0();
    }
    else if(led[1] == 1)
    {
      animation1();
    }
    else if(led[1] == 2)
    {
      animation2();
    }
    turnAllOff();
    turnAllOffToo();
  }
  else if (trueState[led[0]][led[1]])
  {
    turnTrueStateOff(led);
  }
  else
  {
    turnTrueStateOn(led);
  }
}
//turns off led
void turnOff(int led[])
{
  ledState[led[0]][led[1]] = false;
  if (!isRowOn(led[0]))
  {
    rowState[led[0]] = false;
  }
  if (!isColumnOn(led[1]))
  {
    columnState[led[1]] = false;
  }
  updateLeds();
}

void turnTrueStateOff(int led[])
{
  trueState[led[0]][led[1]] = false;
}
void turnTrueStateOn(int led[])
{
  trueState[led[0]][led[1]] = true;
}

void turnRowOn(int row)
{
  int ledtoturnon[] = {row,0};
  for(int i = 0; i <9; i++)
  {
    ledtoturnon[1] = i;
      turnTrueStateOn(ledtoturnon);
  }
}
void turnRowOff(int row)
{
  int ledtoturnoff[] = {row,0};
  for(int i = 0; i <9; i++)
  {
    ledtoturnoff[1] = i;
      turnTrueStateOff(ledtoturnoff);
  }
}
void turnColOn(int col)
{
  int ledtoturnon[] = {0,col};
  for(int i = 0; i <9; i++)
  {
    ledtoturnon[0] = i;
      turnTrueStateOn(ledtoturnon);
  }
}
void turnColOff(int col)
{
  int ledtoturnoff[] = {0,col};
  for(int i = 0; i <9; i++)
  {
    ledtoturnoff[0] = i;
      turnTrueStateOff(ledtoturnoff);
  }
}

void turnAllOff()
{
  int ledtoturnoff[] = {0,0};
  for( int i = 0; i < 9; i++)
  {
    for(int j = 0; j < 9; j++)
    {
      ledtoturnoff[0] = i;
      ledtoturnoff[1] = j;
      turnTrueStateOff(ledtoturnoff);
    }
  }
}
//turns off all leds and refreshes the leds to actually turn off the one led issue
void turnAllOffToo()
{
  int ledtoturnoff[] = {0,0};
  for( int i = 0; i < 9; i++)
  {
    for(int j = 0; j < 9; j++)
    {
      ledtoturnoff[0] = i;
      ledtoturnoff[1] = j;
      turnOff(ledtoturnoff);
    }
  }
  updateLeds();
}
void turnAllOn()
{
  int ledtoturnon[] = {0,0};
  for( int i = 0; i < 9; i++)
  {
    for(int j = 0; j < 9; j++)
    {
      ledtoturnon[0] = i;
      ledtoturnon[1] = j;
      turnTrueStateOn(ledtoturnon);
    }
  }
}

void turnOn(int led[])
{
  ledState[led[0]][led[1]] = true;
  rowState[led[0]] = true;
  columnState[led[1]] = true;
  updateLeds();
}
//checks to see if a particular row/column is on
boolean isRowOn(int row)
{
  boolean isOn = false;
  for(int i = 0; i <= 8; i++)
  {
    if(ledState[row][i] == true)
    {
      isOn = true;
    }
  }
  return isOn;
}
boolean isColumnOn(int column)
{
  boolean isOn = false;
  for(int j = 0; j <= 8; j++)
  {
    if(ledState[j][column] == true)
    {
      isOn = true;
    }
  }
  return isOn;
}

//updates the reg array based on curent led state values
void updateLeds()
{
  updateReg1();
  updateReg2();
  updateReg3();
}

void updateReg1()
{
  reg[0] = 0;
  int power = 1;
  for (int x = 0; x <= 8; x++)
  {
    if(rowState[x])
    {
      reg[0] += power;
    }
    power = power * 2;
  }
}

void updateReg2()
{
  reg[1] = 0;
  int power = 1;
  if (rowState[8])
  {
    reg[1] += power;
  }
   power = power * 2;
  for (int x = 0; x <= 7; x++)
  {
    if (!columnState[x])
    {
      reg[1] += power;
    }

    power = power * 2;
  }
}

void updateReg3()
{
  reg[2] = 0;
  int power = 1;
  for (int x = 0; x < 2; x ++)
  {
    if(!columnState[x+7])
    {
      reg[2] += power;
    }

    power = power * 2;
  }
}

