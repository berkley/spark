/*
TODO
  x Persist data to SVRAM
  x track more than one tap (pin)
  x change LED color over range of full -> empty
  x beer flowing animation
  * API endpoints:
    x reset
    x get data
    * set tap name
    * animation (beer) type

*/
// This #include statement was automatically added by the Particle IDE.
// #include "neopixel/neopixel.h"
#include "neopixel.h"

//photon has 2048 bytes of emulated EEPROM
#define EEPROM_ADDR_T0 0 //400 bytes per tap
#define EEPROM_ADDR_T1 400
#define EEPROM_ADDR_T2 800

#define PIXEL_TYPE WS2812B
#define PIXEL_COUNT 14

struct TapObject
{
  int pulseCount;
  int maxPulseCount;
  int instantPulseCount;
  int previousPulseCount;
  int flowing;
  int percentFull;
  int currentSecond;
  int previousSecond;
  int animationStep;
  int flowPin;
  int ledPin;
};

//initialize taps
TapObject T0 = {1000, 1000, 0, 0, 0, 100, 1, 0, 0, D6, D3};
TapObject T1 = {1000, 1000, 0, 0, 0, 100, 1, 0, 0, D5, D2};
TapObject T2 = {1000, 1000, 0, 0, 0, 100, 1, 0, 0, D4, D1};

int ledPin = D7;

Adafruit_NeoPixel stripT0 = Adafruit_NeoPixel(PIXEL_COUNT, T0.ledPin, PIXEL_TYPE);
Adafruit_NeoPixel stripT1 = Adafruit_NeoPixel(PIXEL_COUNT, T1.ledPin, PIXEL_TYPE);
Adafruit_NeoPixel stripT2 = Adafruit_NeoPixel(PIXEL_COUNT, T2.ledPin, PIXEL_TYPE);

int flowTapPin0Value = 0;
int flowTapPin1Value = 0;
int flowTapPin2Value = 0; 

String *strArr = new String[20];

int handlePost(String command);

void setup()
{
  Serial.begin(9600);
  Serial.println("SETUP");
  stripT0.begin();
  stripT1.begin();
  stripT2.begin();
  //set strips pixel 0 to red until setup is done
  stripT0.setPixelColor(0, stripT0.Color(255,0,0));
  stripT1.setPixelColor(0, stripT0.Color(255,0,0));
  stripT2.setPixelColor(0, stripT0.Color(255,0,0));
  stripT0.show();
  stripT1.show();
  stripT2.show();

  //pin modes
  pinMode(T0.flowPin, INPUT_PULLDOWN);
  pinMode(T1.flowPin, INPUT_PULLDOWN);
  pinMode(T2.flowPin, INPUT_PULLDOWN);
  
  pinMode(ledPin, OUTPUT);
  
  //read any objects set in the EEPROM
  EEPROM.get(EEPROM_ADDR_T0, T0);
  EEPROM.get(EEPROM_ADDR_T1, T1);
  EEPROM.get(EEPROM_ADDR_T2, T2);

  //https://api.particle.io/v1/devices/<device_id>/<variable_name>?access_token=<access_token>
  Particle.variable("maxPCntT0", &T0.maxPulseCount, INT);
  Particle.variable("instPCntT0", &T0.instantPulseCount, INT);
  Particle.variable("pulseCntT0", &T0.pulseCount, INT);
  Particle.variable("perFullT0", &T0.percentFull, INT);

  Particle.variable("maxPCntT1", &T1.maxPulseCount, INT);
  Particle.variable("instPCntT1", &T1.instantPulseCount, INT);
  Particle.variable("pulseCntT1", &T1.pulseCount, INT);
  Particle.variable("perFullT1", &T0.percentFull, INT);

  Particle.variable("maxPCntT2", &T2.maxPulseCount, INT);
  Particle.variable("instPCntT2", &T2.instantPulseCount, INT);
  Particle.variable("pulseCntT2", &T2.pulseCount, INT);
  Particle.variable("perFullT2", &T2.percentFull, INT);

  //cloud functions - handle a cloud POST(data)
  //curl https://api.particle.io/v1/devices/<device_id>/post \
  // -d access_token=<access_token> \
  // -d "args=<command>,<data0>,<dataN>"
  Particle.function("post", handlePost);

  //set pixel 0 to green when setup is done
  stripT0.setPixelColor(0, stripT0.Color(0,255,0));
  stripT1.setPixelColor(0, stripT0.Color(0,255,0));
  stripT2.setPixelColor(0, stripT0.Color(0,255,0));
  stripT0.show();
  stripT1.show();
  stripT2.show();

  Serial.println("SETUP DONE");
}

int handlePost(String postData)
{
  String* args = stringSplit(postData, ',');
  String command = args[0];
  Serial.print("postData:");
  Serial.println(postData);
  
  if(command.equals("reset"))
  { //reset data
    //args[1] == tap number to reset (0-3)
    //this function resets pulseCount to maxPulseCount and writes the EEPROM
    return resetPulseCount(args[1].toInt());
  }
  else if(command.equals("setCalibration"))
  {
    //args[1] == tap you want to set (0-3)
    //args[2] == calibration value (total number of pulses in a keg)
    setCalibration(args[1].toInt(), args[2].toInt());
  }

  return 1;
}

void loop()
{
  pinValT0();
  pinValT1();
  pinValT2();

  Serial.println("pulseCountT0: " + String(T0.pulseCount));
  Serial.println("pulseCountT1: " + String(T1.pulseCount));
  Serial.println("pulseCountT2: " + String(T2.pulseCount));
  
  
  float percentT0 = (float)T0.pulseCount / (float)T0.maxPulseCount;
  T0.percentFull = (int)(percentT0 * 100.0f);
  if(T0.percentFull > 100)
    T0.percentFull = 100;
  EEPROM.put(EEPROM_ADDR_T0, T0);

  float percentT1 = (float)T1.pulseCount / (float)T1.maxPulseCount;
  T1.percentFull = (int)(percentT1 * 100.0f);
  if(T1.percentFull > 100)
    T1.percentFull = 100;
  EEPROM.put(EEPROM_ADDR_T1, T1);

  float percentT2 = (float)T2.pulseCount / (float)T2.maxPulseCount;
  T2.percentFull = (int)(percentT2 * 100.0f);
  if(T2.percentFull > 100)
    T2.percentFull = 100;
  EEPROM.put(EEPROM_ADDR_T2, T2);

  Serial.println("flowingT0: " + String(T0.flowing));
  Serial.println("flowingT1: " + String(T1.flowing));
  Serial.println("flowingT2: " + String(T2.flowing));

  if(T0.flowing)
  {
    animateTap(0);
  }
  else
  {
    if(T0.pulseCount > T0.maxPulseCount)
      T0.pulseCount = T0.maxPulseCount;
    EEPROM.put(EEPROM_ADDR_T0, T0);
    recalculateTapColor(0);
  }

  if(T1.flowing)
  {
    animateTap(1);
  }
  else
  {
    if(T1.pulseCount > T1.maxPulseCount)
      T1.pulseCount = T1.maxPulseCount;
    EEPROM.put(EEPROM_ADDR_T1, T1);
    recalculateTapColor(1);
  }

  if(T2.flowing)
  {
    animateTap(2);
  }
  else
  {
    if(T2.pulseCount > T2.maxPulseCount)
      T2.pulseCount = T2.maxPulseCount;
    EEPROM.put(EEPROM_ADDR_T1, T1);
    recalculateTapColor(2);
  }
}

int resetPulseCount(int tap)
{
  if(tap == 0)
  {
    T0.pulseCount = T0.maxPulseCount;
    EEPROM.put(EEPROM_ADDR_T0, T0);
  }
  else if(tap == 1)
  {
    T1.pulseCount = T1.maxPulseCount;
    EEPROM.put(EEPROM_ADDR_T1, T1);
  }
  else if(tap == 2)
  {
    T2.pulseCount = T2.maxPulseCount;
    EEPROM.put(EEPROM_ADDR_T2, T2); 
  }
  
  return 1;
}

int setCalibration(int tap, int val)
{
  if(tap == 0)
  {
    T0.maxPulseCount = val;
    EEPROM.put(EEPROM_ADDR_T0, T0);
  }
  else if(tap == 1)
  {
    T1.maxPulseCount = val;
    EEPROM.put(EEPROM_ADDR_T1, T1);
  }
  else if(tap == 2)
  {
    T2.maxPulseCount = val;
    EEPROM.put(EEPROM_ADDR_T2, T2);
  }
  return 1;
}

void pinValT0()
{
  if(digitalRead(T0.flowPin) == HIGH)
  {
    if(flowTapPin0Value == 0)
    {
      flowTapPin0Value = 1; 
      digitalWrite(ledPin, LOW);
    }
  }
  else
  {
    if(flowTapPin0Value == 1)
    {
      flowTapPin0Value = 0;
      digitalWrite(ledPin, HIGH);
      T0.instantPulseCount++;
      T0.pulseCount--;
      if(T0.pulseCount < 0) //never go below 0
        T0.pulseCount = 0;
    }
  }
  
  T0.currentSecond = Time.second();
  if(T0.currentSecond != T0.previousSecond)
  {
      T0.instantPulseCount = 0;
      if(T0.previousPulseCount != T0.pulseCount)
        T0.flowing = 1;
      else
        T0.flowing = 0;
      T0.previousPulseCount = T0.pulseCount;
      T0.previousSecond = T0.currentSecond;
  }
}

void pinValT1()
{
  if(digitalRead(T1.flowPin) == HIGH)
  {
    if(flowTapPin1Value == 0)
    {
      flowTapPin1Value = 1; 
      digitalWrite(ledPin, LOW);
    }
  }
  else
  {
    if(flowTapPin1Value == 1)
    {
      flowTapPin1Value = 0;
      digitalWrite(ledPin, HIGH);
      T1.instantPulseCount++;
      T1.pulseCount--;
      if(T1.pulseCount < 0) //never go below 0
        T1.pulseCount = 0;
    }
  }
  
  T1.currentSecond = Time.second();
  if(T1.currentSecond != T1.previousSecond)
  {
      T1.instantPulseCount = 0;
      if(T1.previousPulseCount != T1.pulseCount)
        T1.flowing = 1;
      else
        T1.flowing = 0;
      T1.previousPulseCount = T1.pulseCount;
      T1.previousSecond = T1.currentSecond;
  }
}

void pinValT2()
{
  if(digitalRead(T2.flowPin) == HIGH)
  {
    if(flowTapPin2Value == 0)
    {
      flowTapPin2Value = 1; 
      digitalWrite(ledPin, LOW);
    }
  }
  else
  {
    if(flowTapPin2Value == 1)
    {
      flowTapPin2Value = 0;
      digitalWrite(ledPin, HIGH);
      T2.instantPulseCount++;
      T2.pulseCount--;
      if(T2.pulseCount < 0) //never go below 0
        T2.pulseCount = 0;
    }
  }
  
  T2.currentSecond = Time.second();
  if(T2.currentSecond != T2.previousSecond)
  {
      T2.instantPulseCount = 0;
      if(T2.previousPulseCount != T2.pulseCount)
        T2.flowing = 1;
      else
        T2.flowing = 0;
      T2.previousPulseCount = T2.pulseCount;
      T2.previousSecond = T2.currentSecond;
  }
}

void recalculateTapColor(int stripNum)
{ 
  for(int i=0; i<PIXEL_COUNT; i++)
  {
    if(stripNum == 0)
      stripT0.setPixelColor(i, getTapColor(&T0.percentFull));
    else if(stripNum == 1)
      stripT1.setPixelColor(i, getTapColor(&T1.percentFull));
    else if(stripNum == 2)
      stripT2.setPixelColor(i, getTapColor(&T2.percentFull));
  }

  if(stripNum == 0)
    stripT0.show();
  else if(stripNum == 1)
    stripT1.show();
  else if(stripNum == 2)
    stripT2.show();
}

int getTapColor(int *percentFull)
{
    //change the color of the tap based on percentFull
    //as percentFull goes from 100 to 0, gradiate 
    //from green (0, 255, 0)
    //to red     (255, 0, 0)
    //the middle (yellow) is (255, 255, 0)
    float multiplier = (255.0f + 255.0f) / 100.0f;
    int value = multiplier * *percentFull;
    int red;
    int green;
    //   Serial.print("value:");
    //   Serial.println(value);
    if(value > 255)
    { //add to the red
        green = 255;
        red = 255 - value;
    }
    else
    { //subtract from green
        green = value;
        red = 255;
    }
    
    return stripT0.Color(red, green, 0);
}

void animateTap(int stripNum)
{ //do the flow animation
  if(T0.animationStep >= PIXEL_COUNT)
    T0.animationStep = 0;
  if(T1.animationStep >= PIXEL_COUNT)
    T1.animationStep = 0;
  if(T2.animationStep >= PIXEL_COUNT)
    T2.animationStep = 0;

  if(stripNum == 0)
  {
    setAllPixelsOff(0);
    stripT0.setPixelColor((PIXEL_COUNT - 1) - T0.animationStep, getTapColor(&T0.percentFull));
    stripT0.show();
    T0.animationStep++;
  }
  else if(stripNum == 1)
  {
    setAllPixelsOff(1);
    stripT1.setPixelColor((PIXEL_COUNT - 1) - T1.animationStep, getTapColor(&T1.percentFull));
    stripT1.show();
    T1.animationStep++;
  }
  else if(stripNum == 2)
  {
    setAllPixelsOff(2);
    stripT2.setPixelColor((PIXEL_COUNT - 1) - T2.animationStep, getTapColor(&T2.percentFull));
    stripT2.show();
    T2.animationStep++;
  }
  
  delay(100);
}

void setAllPixelsOff(int stripNum)
{
  if(stripNum == 0)
  {
    for(int i=0; i<PIXEL_COUNT; i++)
    {
      stripT0.setPixelColor(i, stripT0.Color(0,0,0));
    }
    stripT0.show();
  }
  else if(stripNum == 1)
  {
    for(int i=0; i<PIXEL_COUNT; i++)
    {
      stripT1.setPixelColor(i, stripT0.Color(0,0,0));
    }
    stripT1.show();
  }
  else if(stripNum == 2)
  {
    for(int i=0; i<PIXEL_COUNT; i++)
    {
      stripT2.setPixelColor(i, stripT0.Color(0,0,0));
    }
    stripT2.show();
  }
}

String* stringSplit(String s, char delim)
{
    int arrcnt = 0;
    String token = "";
    for(uint8_t i=0; i<s.length(); i++)
    {
        char c = s.charAt(i);
        if(c == delim)
        {
            strArr[arrcnt] = token;
            arrcnt++;
            token = "";
        }
        else
        {
            token.concat(String(c));
        }
    }
    strArr[arrcnt] = token;
    return strArr;
}
