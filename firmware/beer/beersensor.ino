/*
TODO
  x Persist data to SVRAM
  * track more than one tap (pin)
  * change LED color over range of full -> empty
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
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B
#define PIXEL_COUNT 5

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
int pulseCount = 0;
int maxPulseCount = 10000; //bogus init value
int percentFull = 0;
int currentSecond = 1;
int previousSecond = 0;

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
  strip.show();

  //get persistent values
  pulseCount = EEPROM.read(EEPROM_ADDR_PULSE_COUNT);
  maxPulseCount = EEPROM.read(EEPROM_ADDR_MAX_PULSE_COUNT);
  percentFull = EEPROM.read(EEPROM_ADDR_PERCENT_FULL);

  //pin modes
  pinMode(pin0, INPUT_PULLDOWN);
  pinMode(pin1, INPUT_PULLDOWN);
  pinMode(pin2, INPUT_PULLDOWN);
  pinMode(pin3, INPUT_PULLDOWN);
  pinMode(pin4, INPUT_PULLDOWN);
  pinMode(pin5, INPUT_PULLDOWN);
  pinMode(pin6, INPUT_PULLDOWN);

  pinMode(ledPin, OUTPUT);

  //cloud variables
  Spark.variable(pin0Name, &pin0Value, INT);
  Spark.variable(pin1Name, &pin1Value, INT);
  Spark.variable(pin2Name, &pin2Value, INT);
  Spark.variable(pin3Name, &pin3Value, INT);
  Spark.variable(pin4Name, &pin4Value, INT);
  Spark.variable(pin5Name, &pin5Value, INT);
  Spark.variable(pin6Name, &pin6Value, INT);

  //https://api.particle.io/v1/devices/<device_id>/<variable_name>?access_token=<access_token>
  Spark.variable("instantPulseCount", &instantPulseCount, INT);
  Spark.variable("pulseCount", &pulseCount, INT);
  Spark.variable("maxPulseCount", &maxPulseCount, INT);
  Spark.variable("percentFull", &percentFull, INT);

  //cloud functions - handle a cloud POST(data)
  //curl https://api.particle.io/v1/devices/<device_id>/post \
  // -d access_token=<access_token> \
  // -d "args=<command>,<data0>,<dataN>"
  Spark.function("post", handlePost);
}

int handlePost(String postData)
{
  String* args = stringSplit(postData, ',');
  String command = args[0];
  if(command.equals("reset"))
  { //reset data
    return resetPulseCount();
  }
  else if(command.equals("setCalibration"))
  {
    //takes an int as an argument.  args[1] should be the total number of pulses in a given keg.
    //i.e. pulseCount / args[1] == percentFull
    setCalibration(args[1].toInt());
  }
  else if(command.equals("setPinName"))
  {

  }
  else if(command.equals("setPinAnimation"))
  {

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

  EEPROM.update(EEPROM_ADDR_PULSE_COUNT, pulseCount);
  float percent = (float)pulseCount / (float)maxPulseCount;
  percentFull = (int)(percent * 100.0f);
  EEPROM.update(EEPROM_ADDR_PERCENT_FULL, percentFull);
}

int getPulseCount()
{
  return pulseCount;
}

int resetPulseCount()
{
  pulseCount = 0;
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
      pulseCount++;
    }
  }
  
  
  currentSecond = Time.second();
//   Serial.println("CurrentSecond: " + String(currentSecond));
  if(currentSecond != previousSecond)
  {
      setPixels(instantPulseCount);
      instantPulseCount = 0;
      previousSecond = currentSecond;
  }
  
//   Serial.println(pinName + ":" + String(*pinValue));
}

void analogVal(int pin, int* pinValue, String pinName)
{
    *pinValue = analogRead(pin);  // read the input pin
    // Serial.println(pinName + ":" + String(*pinValue));
    digitalWrite(ledPin, *pinValue/16);
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
