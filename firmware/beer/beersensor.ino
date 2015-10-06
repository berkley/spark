/*
TODO
  x Persist data to SVRAM
  * track more than one tap (pin)
  x change LED color over range of full -> empty
  x beer flowing animation
  * API endpoints:
    x reset
    x get data
    x set calibration
    * set tap name
    * animation (beer) type
*/

// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

#define EEPROM_ADDR_PULSE_COUNT 1
#define EEPROM_ADDR_MAX_PULSE_COUNT 2
#define EEPROM_ADDR_PERCENT_FULL 3
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B
#define PIXEL_COUNT 7

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

int ledPin = D7;

int pin0 = A0;
int pin1 = D1;
int pin2 = A2;
int pin3 = D3;
int pin4 = A4;
int pin5 = D5;
int pin6 = D6;

int pin0Value = 0;
int pin1Value = 0;
int pin2Value = 0; 
int pin3Value = 0;
int pin4Value = 0;
int pin5Value = 0;
int pin6Value = 0;

int instantPulseCount = 0;
int previousPulseCount = 0;
bool flowing = false;
int pulseCount = 200;
int maxPulseCount = 200; //bogus init value //real value for chad's fridge left tap: 45 pulses / qt = 900 pulses for a 5 gal keg
int percentFull = 100;
int currentSecond = 1;
int previousSecond = 0;
int animationStep = 0;
String *strArr = new String[20];

const char* pin0Name = "pin0Value";
const char* pin1Name = "pin1Value";
const char* pin2Name = "pin2Value";
const char* pin3Name = "pin3Value";
const char* pin4Name = "pin4Value";
const char* pin5Name = "pin5Value";
const char* pin6Name = "pin6Value";

int handlePost(String command);

void setup()
{
  Serial.begin(9600);
  strip.begin();
  strip.setPixelColor(0, strip.Color(0,0,255));
  strip.show();

  //get persistent values
//   pulseCount = EEPROM.read(EEPROM_ADDR_PULSE_COUNT);
//   maxPulseCount = EEPROM.read(EEPROM_ADDR_MAX_PULSE_COUNT);
//   percentFull = EEPROM.read(EEPROM_ADDR_PERCENT_FULL);

  //pin modes
  pinMode(pin0, INPUT_PULLDOWN);
  pinMode(pin1, INPUT_PULLDOWN);
  pinMode(pin2, INPUT_PULLDOWN);
  pinMode(pin3, INPUT_PULLDOWN);
  pinMode(pin4, INPUT_PULLDOWN);
  pinMode(pin5, INPUT_PULLDOWN);
  pinMode(pin6, INPUT_PULLDOWN);

  pinMode(ledPin, OUTPUT);
  
  //read any values set in the EEPROM
  maxPulseCount = EEPROM.read(EEPROM_ADDR_MAX_PULSE_COUNT);
  pulseCount = EEPROM.read(EEPROM_ADDR_PULSE_COUNT);
  percentFull = EEPROM.read(EEPROM_ADDR_PERCENT_FULL);

  //cloud variables
//   Spark.variable(pin0Name, &pin0Value, INT);
//   Spark.variable(pin1Name, &pin1Value, INT);
//   Spark.variable(pin2Name, &pin2Value, INT);
//   Spark.variable(pin3Name, &pin3Value, INT);
//   Spark.variable(pin4Name, &pin4Value, INT);
//   Spark.variable(pin5Name, &pin5Value, INT);
  Spark.variable(pin6Name, &pin6Value, INT);

  //https://api.particle.io/v1/devices/<device_id>/<variable_name>?access_token=<access_token>
  Spark.variable("maxPCount", &maxPulseCount, INT);
  Spark.variable("instPCount", &instantPulseCount, INT);
  Spark.variable("pulseCount", &pulseCount, INT);
  Spark.variable("percentFull", &percentFull, INT);

  //cloud functions - handle a cloud POST(data)
  //curl https://api.particle.io/v1/devices/<device_id>/post \
  // -d access_token=<access_token> \
  // -d "args=<command>,<data0>,<dataN>"
  Spark.function("post", handlePost);
  strip.setPixelColor(0, strip.Color(255,0,255));
  strip.show();
}

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
//   pinVal(pin0, &pin0Value, pin0Name);
//   pinVal(pin1, &pin1Value, pin1Name);
//   pinVal(pin3, &pin3Value, pin3Name);
//   pinVal(pin4, &pin4Value, pin4Name);
//   pinVal(pin5, &pin5Value, pin5Name);
  pinVal(pin6, &pin6Value, pin6Name);
  
  float percent = (float)pulseCount / (float)maxPulseCount;
  percentFull = (int)(percent * 100.0f);
  if(percentFull > 100)
    percentFull = 100;
  EEPROM.update(EEPROM_ADDR_PERCENT_FULL, percentFull);

  if(flowing)
  {
    animateTap();
  }
  else
  {
    if(pulseCount > maxPulseCount)
      pulseCount = maxPulseCount;
    EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
    recalculateTapColor();
  }
}

int resetPulseCount()
{
  pulseCount = maxPulseCount;
  EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
  return 1;
}

int setCalibration(int val)
{
  maxPulseCount = val;
  EEPROM.update(EEPROM_ADDR_MAX_PULSE_COUNT, maxPulseCount);
}

void pinVal(int pin, int* pinValue, String pinName)
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
      // setPixels(instantPulseCount);
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
