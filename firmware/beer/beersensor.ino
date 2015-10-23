/*
TODO
  x Persist data to SVRAM
  * track more than one tap (pin)
  x change LED color over range of full -> empty
  x beer flowing animation
  * API endpoints:
    x reset
    x get data
    * set tap name
    * animation (beer) type

*/
// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

#define EEPROM_ADDR_PULSE_COUNT 1
#define EEPROM_ADDR_MAX_PULSE_COUNT 2
#define EEPROM_ADDR_PERCENT_FULL 3
#define PIXEL_TYPE WS2812B
#define PIXEL_COUNT 14

int ledPin = D7;

int flowTapPin0 = D6;
int flowTapPin1 = D5;
int flowTap2Pin = D4;

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
bool flowingT0 = false;
int pulseCountT0 = 200;
int maxPulseCountT0 = 200; //calibration value

//tap 1 values
int instantPulseCountT1 = 0;
int previousPulseCountT1 = 0;
bool flowingT1 = false;
int pulseCountT1 = 200;
int maxPulseCountT1 = 200; //calibration value

//tap 3 values
int instantPulseCountT2 = 0;
int previousPulseCountT2 = 0;
bool flowingT2 = false;
int pulseCountT2 = 200;
int maxPulseCountT2 = 200; //calibration value

int percentFullT0 = 100;
int percentFullT1 = 100;
int percentFullT2 = 100;

int currentSecond = 1;
int previousSecond = 0;
int animationStep = 0;
String *strArr = new String[20];

const char* flowTapPin0Name = "flowTapPin0Val";
const char* flowTapPin1Name = "flowTapPin1Val";
const char* flowTapPin2Name = "flowTapPin2Val";

int handlePost(String command);

void setup()
{
  Serial.begin(9600);
  stripT0.begin();
  stripT1.begin();
  stripT2.begin();
  //set strips pixel 0 to red until setup is done
  stripT0.setPixelColor(0, strip.Color(255,0,0));
  stripT1.setPixelColor(0, strip.Color(255,0,0));
  stripT2.setPixelColor(0, strip.Color(255,0,0));
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
  Spark.variable("maxPCountTap0", &maxPulseCountT0, INT);
  Spark.variable("instPCountTap0", &instantPulseCountT0, INT);
  Spark.variable("pulseCountTap0", &pulseCountT0, INT);
  Spark.variable("percentFullTap0", &percentFullT0, INT);

  Spark.variable("maxPCountTap1", &maxPulseCountT1, INT);
  Spark.variable("instPCountTap1", &instantPulseCountT1, INT);
  Spark.variable("pulseCountTap1", &pulseCountT1, INT);
  Spark.variable("percentFullTap1", &percentFullT1, INT);

  Spark.variable("maxPCountTap2", &maxPulseCountT2, INT);
  Spark.variable("instPCountTap2", &instantPulseCountT2, INT);
  Spark.variable("pulseCountTap2", &pulseCountT2, INT);
  Spark.variable("percentFullTap2", &percentFullT2, INT);

  //cloud functions - handle a cloud POST(data)
  //curl https://api.particle.io/v1/devices/<device_id>/post \
  // -d access_token=<access_token> \
  // -d "args=<command>,<data0>,<dataN>"
  Spark.function("post", handlePost);

  //set pixel 0 to green when setup is done
  stripT0.setPixelColor(0, strip.Color(0,255,0));
  stripT1.setPixelColor(0, strip.Color(0,255,0));
  stripT2.setPixelColor(0, strip.Color(0,255,0));
  stripT0.show();
  stripT1.show();
  stripT2.show();
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
}

void loop()
{
  pinVal(flowTapPin0, &flowTapPin0Value, flowTapPin0Name);
  pinVal(flowTapPin1, &flowTapPin1Value, flowTapPin1Name);
  pinVal(flowTapPin2, &flowTapPin2Value, flowTapPin2Name);
  
  float percent = (float)pulseCountT0 / (float)maxPulseCountT0;
  percentFullT0 = (int)(percent * 100.0f);
  if(percentFullT0 > 100)
    percentFullT0 = 100;
  // EEPROM.update(EEPROM_ADDR_PERCENT_FULL, percentFull);

  if(flowingT0)
  {
    animateTap(stripT0);
  }
  else
  {
    if(pulseCountT0 > maxPulseCountT0)
      pulseCountT0 = maxPulseCountT0;
    // EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
    recalculateTapColor(stripT0);
  }

  if(flowingT1)
  {
    animateTap(stripT1);
  }
  else
  {
    if(pulseCountT1 > maxPulseCountT1)
      pulseCountT1 = maxPulseCountT1;
    // EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
    recalculateTapColor(stripT1);
  }

  if(flowingT2)
  {
    animateTap(stripT2);
  }
  else
  {
    if(pulseCountT0 > maxPulseCountT2)
      pulseCountT0 = maxPulseCountT2;
    // EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
    recalculateTapColor(stripT2);
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
}

void pinVal(int pin, int* pinValue, String pinName, int* pulseCount, 
            int* previousPulseCount, int* instantPulseCount, bool* flowing)
{
  if(digitalRead(pin) == HIGH)
  {
    if(*pinValue == 0)
    {
      *pinValue = 1; 
      digitalWrite(ledPin, LOW);
    }
  }
  else
  {
    if(*pinValue == 1)
    {
      *pinValue = 0;
      digitalWrite(ledPin, HIGH);
      instantPulseCount++;
      pulseCount--;
      if(pulseCount < 0) //never go below 0
        pulseCount = 0;
    }
  }
  
  currentSecond = Time.second();
  if(currentSecond != previousSecond)
  {
      instantPulseCount = 0;
      if(previousPulseCount != pulseCount)
        flowing = true;
      else
        flowing = false;
      previousPulseCount = pulseCount;
      previousSecond = currentSecond;
  }
}

void analogVal(int pin, int* pinValue, String pinName)
{
  *pinValue = analogRead(pin);  // read the input pin
  // Serial.println(pinName + ":" + String(*pinValue));
  digitalWrite(ledPin, *pinValue/16);
}

void recalculateTapColor()
{ 
  for(int i=0; i<PIXEL_COUNT; i++)
  {
    strip.setPixelColor(i, getTapColor());
  }
  strip.show();
}

int getTapColor()
{
    //change the color of the tap based on percentFull
    //as percentFull goes from 100 to 0, gradiate 
    //from green (0, 255, 0)
    //to red     (255, 0, 0)
    //the middle (yellow) is (255, 255, 0)
    float multiplier = (255.0f + 255.0f) / 100.0f;
    int value = multiplier * percentFull;
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
    
    return strip.Color(red, green, 0);
}

void animateTap()
{ //do the flow animation
  if(animationStep >= PIXEL_COUNT)
    animationStep = 0;

  setAllPixelsOff();
  strip.setPixelColor((PIXEL_COUNT - 1) - animationStep, getTapColor());
  strip.show();
  delay(100);
  animationStep++;
}

void setAllPixelsOff()
{
  for(int i=0; i<PIXEL_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
}

void setPixels(int instantPulseCount)
{
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.setPixelColor(1, strip.Color(0, 0, 0));
    strip.setPixelColor(2, strip.Color(0, 0, 0));
    strip.setPixelColor(3, strip.Color(0, 0, 0));
    strip.setPixelColor(4, strip.Color(0, 0, 0));
    // Serial.println("PulseCount: " + String(instantPulseCount));
    if(instantPulseCount > 0)
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    if(instantPulseCount > 100)
    strip.setPixelColor(1, strip.Color(0, 255, 0));
    if(instantPulseCount > 200)
    strip.setPixelColor(2, strip.Color(255, 165, 0));
    if(instantPulseCount > 300)
    strip.setPixelColor(3, strip.Color(255, 165, 0));
    if(instantPulseCount > 400)
    strip.setPixelColor(4, strip.Color(255, 0, 0));
    strip.show();
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
