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

#define EEPROM_ADDR_PULSE_COUNT 1
#define EEPROM_ADDR_MAX_PULSE_COUNT 2
#define EEPROM_ADDR_PERCENT_FULL 3
#define PIXEL_TYPE WS2812B
#define PIXEL_COUNT 14

int ledPin = D7;

int flowTapPin0 = D6;
int flowTapPin1 = D5;
int flowTapPin2 = D4;

int ledTapPin0 = D3;
int ledTapPin1 = D2;
int ledTapPin2 = D1;

Adafruit_NeoPixel stripT0 = Adafruit_NeoPixel(PIXEL_COUNT, ledTapPin0, PIXEL_TYPE);
Adafruit_NeoPixel stripT1 = Adafruit_NeoPixel(PIXEL_COUNT, ledTapPin1, PIXEL_TYPE);
Adafruit_NeoPixel stripT2 = Adafruit_NeoPixel(PIXEL_COUNT, ledTapPin2, PIXEL_TYPE);

int flowTapPin0Value = 0;
int flowTapPin1Value = 0;
int flowTapPin2Value = 0; 

//tap 0 values
int instantPulseCountT0 = 0;
int previousPulseCountT0 = 0;
int flowingT0 = 0;
int pulseCountT0 = 200;
int maxPulseCountT0 = 200; //calibration value

//tap 1 values
int instantPulseCountT1 = 0;
int previousPulseCountT1 = 0;
int flowingT1 = 0;
int pulseCountT1 = 200;
int maxPulseCountT1 = 200; //calibration value

//tap 3 values
int instantPulseCountT2 = 0;
int previousPulseCountT2 = 0;
int flowingT2 = 0;
int pulseCountT2 = 200;
int maxPulseCountT2 = 200; //calibration value

int percentFullT0 = 100;
int percentFullT1 = 100;
int percentFullT2 = 100;

int currentSecondT0 = 1;
int previousSecondT0 = 0;
int animationStepT0 = 0;
int currentSecondT1 = 1;
int previousSecondT1 = 0;
int animationStepT1 = 0;
int currentSecondT2 = 1;
int previousSecondT2 = 0;
int animationStepT2 = 0;

String *strArr = new String[20];

const char* flowTapPin0Name = "flowTapPin0Val";
const char* flowTapPin1Name = "flowTapPin1Val";
const char* flowTapPin2Name = "flowTapPin2Val";

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
  pinMode(flowTapPin0, INPUT_PULLDOWN);
  pinMode(flowTapPin1, INPUT_PULLDOWN);
  pinMode(flowTapPin2, INPUT_PULLDOWN);
  
  pinMode(ledPin, OUTPUT);
  
  //read any values set in the EEPROM
  // maxPulseCount = EEPROM.read(EEPROM_ADDR_MAX_PULSE_COUNT);
  // pulseCount = EEPROM.read(EEPROM_ADDR_PULSE_COUNT);
  // percentFull = EEPROM.read(EEPROM_ADDR_PERCENT_FULL);

  //https://api.particle.io/v1/devices/<device_id>/<variable_name>?access_token=<access_token>
  Particle.variable("maxPCntT0", &maxPulseCountT0, INT);
  Particle.variable("instPCntT0", &instantPulseCountT0, INT);
  Particle.variable("pulseCntT0", &pulseCountT0, INT);
  Particle.variable("perFullT0", &percentFullT0, INT);

  Particle.variable("maxPCntT1", &maxPulseCountT1, INT);
  Particle.variable("instPCntT1", &instantPulseCountT1, INT);
  Particle.variable("pulseCntT1", &pulseCountT1, INT);
  Particle.variable("perFullT1", &percentFullT1, INT);

  // Particle.variable("maxPCntT2", &maxPulseCountT2, INT);
  // Particle.variable("instPCntT2", &instantPulseCountT2, INT);
  // Particle.variable("pulseCntT2", &pulseCountT2, INT);
  // Particle.variable("perFullT2", &percentFullT2, INT);

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

//TODO: make this work for 3 taps
int handlePost(String postData)
{
  String* args = stringSplit(postData, ',');
  String command = args[0];
  Serial.print("postData:");
  Serial.println(postData);
  
  if(command.equals("reset"))
  { //reset data
    Serial.println("reset pulse count");
    return resetPulseCount();
  }
  else if(command.equals("setCalibration"))
  {
    //takes an int as an argument.  args[1] should be the total number of pulses in a given keg.
    //i.e. pulseCount / args[1] == percentFull
    setCalibration(args[1].toInt());
  }

  return 1;
}

void loop()
{
  //void pinVal(int pin, int* pinValue, String pinName, int* pulseCount, 
            // int* previousPulseCount, int* instantPulseCount, int* flowing)
  // pinVal(flowTapPin0, &flowTapPin0Value, &pulseCountT0, 
  //        &previousPulseCountT0, &instantPulseCountT0, &flowingT0);
  // pinVal(flowTapPin1, &flowTapPin1Value, &pulseCountT1, 
  //        &previousPulseCountT1, &instantPulseCountT1, &flowingT1);
  // pinVal(flowTapPin2, &flowTapPin2Value, &pulseCountT2, 
  //        &previousPulseCountT2, &instantPulseCountT2, &flowingT2);

  pinValT0(flowTapPin0);
  pinValT1(flowTapPin1);
  pinValT2(flowTapPin2);

  Serial.println("pulseCountT0: " + String(pulseCountT0));
  Serial.println("pulseCountT1: " + String(pulseCountT1));
  Serial.println("pulseCountT2: " + String(pulseCountT2));
  
  
  float percentT0 = (float)pulseCountT0 / (float)maxPulseCountT0;
  percentFullT0 = (int)(percentT0 * 100.0f);
  if(percentFullT0 > 100)
    percentFullT0 = 100;
  // EEPROM.update(EEPROM_ADDR_PERCENT_FULL, percentFull);

  float percentT1 = (float)pulseCountT1 / (float)maxPulseCountT1;
  percentFullT1 = (int)(percentT1 * 100.0f);
  if(percentFullT1 > 100)
    percentFullT1 = 100;
  // EEPROM.update(EEPROM_ADDR_PERCENT_FULL, percentFull);

  float percentT2 = (float)pulseCountT2 / (float)maxPulseCountT2;
  percentFullT2 = (int)(percentT2 * 100.0f);
  if(percentFullT2 > 100)
    percentFullT2 = 100;
  // EEPROM.update(EEPROM_ADDR_PERCENT_FULL, percentFull);  

  Serial.println("flowingT0: " + String(flowingT0));
  Serial.println("flowingT1: " + String(flowingT1));
  Serial.println("flowingT2: " + String(flowingT2));

  if(flowingT0)
  {
    animateTap(0);
  }
  else
  {
    if(pulseCountT0 > maxPulseCountT0)
      pulseCountT0 = maxPulseCountT0;
    // EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
    recalculateTapColor(0);
  }

  if(flowingT1)
  {
    animateTap(1);
  }
  else
  {
    if(pulseCountT1 > maxPulseCountT1)
      pulseCountT1 = maxPulseCountT1;
    // EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
    recalculateTapColor(1);
  }

  if(flowingT2)
  {
    animateTap(2);
  }
  else
  {
    if(pulseCountT2 > maxPulseCountT2)
      pulseCountT2 = maxPulseCountT2;
    // EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
    recalculateTapColor(2);
  }
}

int resetPulseCount()
{
  // pulseCount = maxPulseCount;
  // EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
  return 1;
}

int setCalibration(int val)
{
  // maxPulseCount = val;
  // EEPROM.update(EEPROM_ADDR_MAX_PULSE_COUNT, maxPulseCount);
  return 1;
}

void pinValT0(int pin)
{
  if(digitalRead(pin) == HIGH)
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
      instantPulseCountT0++;
      pulseCountT0--;
      if(pulseCountT0 < 0) //never go below 0
        pulseCountT0 = 0;
    }
  }
  
  currentSecondT0 = Time.second();
  if(currentSecondT0 != previousSecondT0)
  {
      instantPulseCountT0 = 0;
      if(previousPulseCountT0 != pulseCountT0)
        flowingT0 = 1;
      else
        flowingT0 = 0;
      previousPulseCountT0 = pulseCountT0;
      previousSecondT0 = currentSecondT0;
  }
}

void pinValT1(int pin)
{
  if(digitalRead(pin) == HIGH)
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
      instantPulseCountT1++;
      pulseCountT1--;
      if(pulseCountT1 < 0) //never go below 0
        pulseCountT1 = 0;
    }
  }
  
  currentSecondT1 = Time.second();
  if(currentSecondT1 != previousSecondT1)
  {
      instantPulseCountT1 = 0;
      if(previousPulseCountT1 != pulseCountT1)
        flowingT1 = 1;
      else
        flowingT1 = 0;
      previousPulseCountT1 = pulseCountT1;
      previousSecondT1 = currentSecondT1;
  }
}

void pinValT2(int pin)
{
  if(digitalRead(pin) == HIGH)
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
      instantPulseCountT2++;
      pulseCountT2--;
      if(pulseCountT2 < 0) //never go below 0
        pulseCountT2 = 0;
    }
  }
  
  currentSecondT2 = Time.second();
  if(currentSecondT2 != previousSecondT2)
  {
      instantPulseCountT2 = 0;
      if(previousPulseCountT2 != pulseCountT2)
        flowingT2 = 1;
      else
        flowingT2 = 0;
      previousPulseCountT2 = pulseCountT2;
      previousSecondT2 = currentSecondT2;
  }
}

void recalculateTapColor(int stripNum)
{ 
  for(int i=0; i<PIXEL_COUNT; i++)
  {
    if(stripNum == 0)
      stripT0.setPixelColor(i, getTapColor(&percentFullT0));
    else if(stripNum == 1)
      stripT1.setPixelColor(i, getTapColor(&percentFullT1));
    else if(stripNum == 2)
      stripT2.setPixelColor(i, getTapColor(&percentFullT2));
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
  if(animationStepT0 >= PIXEL_COUNT)
    animationStepT0 = 0;
  if(animationStepT1 >= PIXEL_COUNT)
    animationStepT1 = 0;
  if(animationStepT2 >= PIXEL_COUNT)
    animationStepT2 = 0;

  if(stripNum == 0)
  {
    setAllPixelsOff(0);
    stripT0.setPixelColor((PIXEL_COUNT - 1) - animationStepT0, getTapColor(&percentFullT0));
    stripT0.show();
    animationStepT0++;
  }
  else if(stripNum == 1)
  {
    setAllPixelsOff(1);
    stripT1.setPixelColor((PIXEL_COUNT - 1) - animationStepT1, getTapColor(&percentFullT1));
    stripT1.show();
    animationStepT1++;
  }
  else if(stripNum == 2)
  {
    setAllPixelsOff(2);
    stripT2.setPixelColor((PIXEL_COUNT - 1) - animationStepT2, getTapColor(&percentFullT2));
    stripT2.show();
    animationStepT2++;
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
