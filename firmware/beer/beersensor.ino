//See the README.md for docs

#include "neopixel.h"

//photon has 2048 bytes of emulated EEPROM
#define EEPROM_ADDR_T0 0 //600 bytes per tap
#define EEPROM_ADDR_T1 600
#define EEPROM_ADDR_T2 1200

#define PIXEL_TYPE WS2812B
#define PIXEL_COUNT 14

#define FLOW_PIN_T0 D6
#define FLOW_PIN_T1 D5
#define FLOW_PIN_T2 D4
#define LED_PIN_T0 D3
#define LED_PIN_T1 D2
#define LED_PIN_T2 D1

struct TapObject
{
  int pulseCount;
  int maxPulseCount;
  int instantPulseCount;
  int flowStartPulseCount;
  int previousPulseCount;
  int flowing;
  int previousFlowing;
  int percentFull;
  int currentSecond;
  int previousSecond;
  int animationStep;
  int flowPin;
  int ledPin;
};

//initialize taps
TapObject T0 = {1000, 1000, 0, 0, 0, 0, 0, 100, 1, 0, 0, FLOW_PIN_T0, LED_PIN_T0};
TapObject T1 = {1000, 1000, 0, 0, 0, 0, 0, 100, 1, 0, 0, FLOW_PIN_T1, LED_PIN_T1};
TapObject T2 = {1000, 1000, 0, 0, 0, 0, 0, 100, 1, 0, 0, FLOW_PIN_T2, LED_PIN_T2};

int ledPin = D7;

Adafruit_NeoPixel stripT0 = Adafruit_NeoPixel(PIXEL_COUNT, LED_PIN_T0, PIXEL_TYPE);
Adafruit_NeoPixel stripT1 = Adafruit_NeoPixel(PIXEL_COUNT, LED_PIN_T1, PIXEL_TYPE);
Adafruit_NeoPixel stripT2 = Adafruit_NeoPixel(PIXEL_COUNT, LED_PIN_T2, PIXEL_TYPE);

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
  Particle.variable("perFullT1", &T1.percentFull, INT);

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
    //set the calibration value (maxPulseCount)
    //args[1] == tap you want to set (0-3)
    //args[2] == calibration value (total number of pulses in a keg)
    setCalibration(args[1].toInt(), args[2].toInt());
  }
  else if(command.equals("setPulseCount"))
  {
    //set the pulseCount for a tap manually
    //args[1] == tap you want to set (0-3)
    //args[2] == calibration value (total number of pulses in a keg)
    setPulseCount(args[1].toInt(), args[2].toInt());
  }

  return 1;
}

void loop()
{
  pinValT0();
  pinValT1();
  pinValT2();
  calculatePercentFull();
  //publishFlowingEvents();
  handleAnimation();
}

void writeToEEPROM()
{
  EEPROM.put(EEPROM_ADDR_T0, T0);
  EEPROM.put(EEPROM_ADDR_T1, T1);
  EEPROM.put(EEPROM_ADDR_T2, T2);
}

void calculatePercentFull()
{
  float percentT0 = (float)T0.pulseCount / (float)T0.maxPulseCount;
  T0.percentFull = (int)(percentT0 * 100.0f);
  if(T0.percentFull > 100)
    T0.percentFull = 100;

  float percentT1 = (float)T1.pulseCount / (float)T1.maxPulseCount;
  T1.percentFull = (int)(percentT1 * 100.0f);
  if(T1.percentFull > 100)
    T1.percentFull = 100;
  
  float percentT2 = (float)T2.pulseCount / (float)T2.maxPulseCount;
  T2.percentFull = (int)(percentT2 * 100.0f);
  if(T2.percentFull > 100)
    T2.percentFull = 100;
}

void handleAnimation()
{
  if(T0.flowing)
  {
    Serial.println("flowingT0: " + String(T0.flowing));
    animateTap(0);
    writeToEEPROM();
  }
  else
  {
    if(T0.pulseCount > T0.maxPulseCount)
      T0.pulseCount = T0.maxPulseCount;
    recalculateTapColor(0);
  }

  if(T1.flowing)
  {
    animateTap(1);
    Serial.println("flowingT1: " + String(T1.flowing));
    writeToEEPROM();
  }
  else
  {
    if(T1.pulseCount > T1.maxPulseCount)
      T1.pulseCount = T1.maxPulseCount;
    recalculateTapColor(1);
  }

  if(T2.flowing)
  {
    animateTap(2);
    Serial.println("flowingT2: " + String(T2.flowing));
    writeToEEPROM();
  }
  else
  {
    if(T2.pulseCount > T2.maxPulseCount)
      T2.pulseCount = T2.maxPulseCount;
    recalculateTapColor(2);
  }
}

void publishFlowingEvents()
{
  if(T0.flowing != T0.previousFlowing)
  {
    if(T0.flowing)
    {
      T0.flowStartPulseCount = T0.pulseCount;
      //publish a start event
      Particle.publish("start-flow-T0", String(T0.maxPulseCount) + 
                       "," + String(T0.pulseCount) + "," + String(T0.percentFull));
    }
    else
    {
      //publish a stop event
      String data = String(T0.maxPulseCount) + 
                       "," + String(T0.pulseCount) + "," + String(T0.percentFull) +
                       "," + String(T0.flowStartPulseCount - T0.pulseCount);
      Serial.println("stop-flow-T0 Data: " + data);
      Particle.publish("stop-flow-T0", data);
    }
  }

  if(T1.flowing != T1.previousFlowing)
  {
    if(T1.flowing)
    {
      T1.flowStartPulseCount = T1.pulseCount;
      //publish a start event
      Particle.publish("start-flow-T1", String(T1.maxPulseCount) + 
                       "," + String(T1.pulseCount) + "," + String(T1.percentFull));
    }
    else
    {
      //publish a stop event
      String data = String(T1.maxPulseCount) + 
                       "," + String(T1.pulseCount) + "," + String(T1.percentFull) +
                       "," + String(T1.flowStartPulseCount - T1.pulseCount);
      Serial.println("stop-flow-T1 Data: " + data);
      Particle.publish("stop-flow-T1", data);
    }
  }

  if(T2.flowing != T2.previousFlowing)
  {
    if(T2.flowing)
    {
      T2.flowStartPulseCount = T2.pulseCount;
      //publish a start event
      Particle.publish("start-flow-T2", String(T2.maxPulseCount) + 
                       "," + String(T2.pulseCount) + "," + String(T2.percentFull));
    }
    else
    {
      String data = String(T2.maxPulseCount) + 
                       "," + String(T2.pulseCount) + "," + String(T2.percentFull) +
                       "," + String(T2.flowStartPulseCount - T2.pulseCount);
      Serial.println("stop-flow-T2 Data: " + data);
      Particle.publish("stop-flow-T2", data);
    }
  }
}

int resetPulseCount(int tap)
{
  if(tap == 0)
  {
    T0.pulseCount = T0.maxPulseCount;
  }
  else if(tap == 1)
  {
    T1.pulseCount = T1.maxPulseCount;
  }
  else if(tap == 2)
  {
    T2.pulseCount = T2.maxPulseCount;
  }
  writeToEEPROM();
  return 1;
}

int setPulseCount(int tap, int val)
{
  if(tap == 0)
  {
    T0.pulseCount = val;
  }
  else if(tap == 1)
  {
    T1.pulseCount = val;
  }
  else if(tap == 2)
  {
    T2.pulseCount = val;
  }
  writeToEEPROM();
  return 1;
}

int setCalibration(int tap, int val)
{
  if(tap == 0)
  {
    T0.maxPulseCount = val;
  }
  else if(tap == 1)
  {
    T1.maxPulseCount = val;
  }
  else if(tap == 2)
  {
    T2.maxPulseCount = val;
  }
  writeToEEPROM();
  return 1;
}

void pinValT0()
{
  T0.previousFlowing = T0.flowing;
  if(digitalRead(FLOW_PIN_T0) == HIGH)
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
      {
        T0.flowing = 1;
      }
      else
      {
        T0.flowing = 0;
      }
      T0.previousPulseCount = T0.pulseCount;
      T0.previousSecond = T0.currentSecond;
  }
}

void pinValT1()
{ 
  T1.previousFlowing = T1.flowing;
  if(digitalRead(FLOW_PIN_T1) == HIGH)
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
      {
        T1.flowing = 1;
      }
      else
      {
        T1.flowing = 0;
      }
      T1.previousPulseCount = T1.pulseCount;
      T1.previousSecond = T1.currentSecond;
  }
}

void pinValT2()
{
  T2.previousFlowing = T2.flowing;
  if(digitalRead(FLOW_PIN_T2) == HIGH)
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
      {
        T2.flowing = 1;
      }
      else
      {
        T2.flowing = 0;
      }
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
