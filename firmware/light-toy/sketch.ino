#include "application.h"
#include "neopixel.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

Adafruit_NeoPixel panel = Adafruit_NeoPixel(64, D0, WS2812B);

void doXEncoderA();
void doXxEncoderB();
void doYEncoderA();
void doyEncoderB();
void allOff();

//X-axis encoder - left side
int xEncoderA = D5;
int xEncoderB = D6;
int xEncoderButtonPin = D1;

//Y-axis encoder - right side
int yEncoderA = D3;
int yEncoderB = D4;
int yEncoderButtonPin = D2;

int potPin0 = A0; //red
int potPin1 = A1; //green
int potPin2 = A2; //blue

volatile bool xA_set = false;
volatile bool xB_set = false;
volatile bool xButton_set = false;
volatile int xEncoderPos = 0;

volatile bool yA_set = false;
volatile bool yB_set = false;
volatile bool yButton_set = false;
volatile int yEncoderPos = 0;

int pixelIndex = 0;
int xprevPos = 0;
int yprevPos = 0;

int blinkState = true;

int MAX_COLOR = 255;

void setup() {
  Serial.begin(9600); 
  delay(1000);

  panel.begin();
  panel.show();
  panel.setBrightness(64);

  pinMode(xEncoderA, INPUT_PULLUP);
  pinMode(xEncoderB, INPUT_PULLUP);
  pinMode(xEncoderButtonPin, INPUT_PULLUP);
  attachInterrupt(xEncoderA, doxEncoderA, CHANGE);
  attachInterrupt(xEncoderB, doxEncoderB, CHANGE);
  attachInterrupt(xEncoderButtonPin, doxEncoderButton, RISING);

  pinMode(yEncoderA, INPUT_PULLUP);
  pinMode(yEncoderB, INPUT_PULLUP);
  pinMode(yEncoderButtonPin, INPUT_PULLUP);
  attachInterrupt(yEncoderA, doyEncoderA, CHANGE);
  attachInterrupt(yEncoderB, doyEncoderB, CHANGE);
  attachInterrupt(yEncoderButtonPin, doyEncoderButton, RISING);

  delay(1000);

  if(digitalRead(xEncoderButtonPin) == LOW)
  {
    Serial.println("Button pressed dupanel startup");
    if (Particle.connected() == false) {
      Particle.connect();
    }
  }

  for(int i=0; i<panel.numPixels(); i++)
  {
    panel.setPixelColor(i, panel.Color(random(255), random(255), random(255)));
    panel.show();
    delay(20);
  }

  makeE(panel.Color(0,0,0));
  delay(100);
  makeE(panel.Color(255,0,0));
  delay(100);
  makeE(panel.Color(0,255,0));
  delay(100);
  makeE(panel.Color(0,0,255));
  delay(100);
  makeE(panel.Color(0,0,0));
}

void loop() {
  if (xprevPos != xEncoderPos) {
    if(xprevPos > xEncoderPos)
      pixelIndex++;
    else
      pixelIndex--;

    xprevPos = xEncoderPos;

    if(pixelIndex >= panel.numPixels())
      pixelIndex = pixelIndex - panel.numPixels();
    else if(pixelIndex < 0)
      pixelIndex = pixelIndex + (panel.numPixels() - 1);

    Serial.println("xxxpixelIndex: " + String(pixelIndex));
    Serial.println("xxxxEncoderPos: " + String(xEncoderPos));
    Serial.println("xxxyEncoderPos: " + String(yEncoderPos));
    panel.setPixelColor(pixelIndex, getRGBPotVal());
    panel.show();
  } else if (yprevPos != yEncoderPos) {
    if(yprevPos > yEncoderPos)
      pixelIndex += 8;
    else 
      pixelIndex -= 8;

    yprevPos = yEncoderPos;

    if(pixelIndex >= panel.numPixels())
      pixelIndex = pixelIndex - panel.numPixels();
    else if(pixelIndex < 0)
      pixelIndex = pixelIndex + (panel.numPixels() - 1);

    Serial.println("yyypixelIndex: " + String(pixelIndex));
    Serial.println("yyyxEncoderPos: " + String(xEncoderPos));
    Serial.println("yyyyEncoderPos: " + String(yEncoderPos));
    panel.setPixelColor(pixelIndex, getRGBPotVal());
    panel.show();
  } else {
    // if(blinkState)
    // {
      panel.setPixelColor(pixelIndex, getRGBPotVal());
    // }
    // else
    // {
    //   panel.setPixelColor(pixelIndex, panel.Color(0,0,0));
    // }
    // blinkState = !blinkState;
    panel.show();
    // delay(20);
  }


}

void xEncoderButtonPushed()
{
    Serial.println("X Button Pushed");
}

void yEncoderButtonPushed()
{
    Serial.println("Y Button Pushed");
}

unsigned long xlastDebounceTime = 0;
unsigned long xdebounceDelay = 50;
int xbuttonState;

void doxEncoderButton()
{
    xbuttonState = digitalRead(xEncoderButtonPin);
    if(xbuttonState == HIGH &&
      (millis() - xlastDebounceTime) > xdebounceDelay)
    {
        xlastDebounceTime = millis();
        xEncoderButtonPushed();
    }
}

void doxEncoderA(){

  if(digitalRead(xEncoderA) != xA_set)
  {
    xA_set = !xA_set; // adjust counter + if A leads B
    Serial.println("xEncoderA: " + String(xA_set) + "  xEncoderB: " + String(xB_set));
    if (xA_set && !xB_set)
    {
      Serial.println("xEncoderA-FIRE");
      xEncoderPos += 1;
    }
  }
}

// Interrupt on B changing state, same as A above
void doxEncoderB(){

  if(digitalRead(xEncoderB) != xB_set) {
    xB_set = !xB_set; //  adjust counter - 1 if B leads A
    Serial.println("xEncoderB: " + String(xB_set) + "  xEncoderA: " + String(xA_set));
    if(xB_set && !xA_set)
    {
      Serial.println("xEncoderB-FIRE");
      xEncoderPos += 1;
    }
  }
}

unsigned long ylastDebounceTime = 0;
unsigned long ydebounceDelay = 50;
int ybuttonState;

void doyEncoderButton()
{
    ybuttonState = digitalRead(yEncoderButtonPin);
    if(ybuttonState == HIGH &&
      (millis() - ylastDebounceTime) > ydebounceDelay)
    {
        ylastDebounceTime = millis();
        yEncoderButtonPushed();
    }
}

void doyEncoderA(){

  if(digitalRead(yEncoderA) != yA_set)
  {
    yA_set = !yA_set; // adjust counter + if A leads B
    Serial.println("yEncoderA: " + String(yA_set) + "  xEncoderB: " + String(yB_set));
    if (yA_set && !yB_set)
    {
      Serial.println("yEncoderA-FIRE");
      yEncoderPos += 1;
    }
  }
}

// Interrupt on B changing state, same as A above
void doyEncoderB(){

  if(digitalRead(yEncoderB) != yB_set) {
    yB_set = !yB_set; //  adjust counter - 1 if B leads A
    Serial.println("yEncoderB: " + String(yB_set) + "  yEncoderA: " + String(yA_set));
    if(yB_set && !yA_set)
    {
      Serial.println("yEncoderB-FIRE");
      yEncoderPos += 1;
    }
  }
}

int getRGBPotVal() {
    int pot0Val = analogRead(potPin0) / 16;
    int pot1Val = analogRead(potPin1) / 16;
    int pot2Val = analogRead(potPin2) / 16;
    /*Serial.println("pot0Val: " + String(pot0Val));
    Serial.println("pot1Val: " + String(pot1Val));
    Serial.println("pot2Val: " + String(pot2Val));*/
    return panel.Color(pot0Val, pot1Val, pot2Val);
}

void panelAllOff()
{
    for(int i=0; i<12; i++)
    {
      panel.setPixelColor(i, panel.Color(0,0,0));
    }
}

void panelSetAll(int color)
{
    for(int i=0; i<12; i++)
    {
        panel.setPixelColor(i, color);
    }
    panel.show();
}

// Input a value 0 to MAX_COLOR to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
    int maxVal = MAX_COLOR;
    if(WheelPos < 85)
    {
     return panel.Color(WheelPos * 3, maxVal - WheelPos * 3, 0);
    }
    else if(WheelPos < 170)
    {
     WheelPos -= 85;
     return panel.Color(maxVal - WheelPos * 3, 0, WheelPos * 3);
    }
    else
    {
     WheelPos -= 170;
     return panel.Color(0, WheelPos * 3, maxVal - WheelPos * 3);
    }
}

void makeE(int color)
{
  panel.setPixelColor(2, color);
  panel.setPixelColor(3, color);
  panel.setPixelColor(4, color);
  panel.setPixelColor(5, color);
  panel.show();
  delay(100);
  panel.setPixelColor(10, color);
  panel.setPixelColor(11, color);
  panel.setPixelColor(12, color);
  panel.setPixelColor(13, color);
  panel.show();
  delay(100);
  panel.setPixelColor(18, color);
  panel.setPixelColor(19, color);
  panel.show();
  panel.setPixelColor(26, color);
  panel.setPixelColor(27, color);
  panel.setPixelColor(28, color);
  panel.show();
  delay(100);
  panel.setPixelColor(34, color);
  panel.setPixelColor(35, color);
  panel.setPixelColor(36, color);
  panel.show();
  delay(100);
  panel.setPixelColor(42, color);
  panel.setPixelColor(43, color);
  panel.show();
  delay(100);
  panel.setPixelColor(50, color);
  panel.setPixelColor(51, color);
  panel.setPixelColor(52, color);
  panel.setPixelColor(53, color);
  panel.show();
  delay(100);
  panel.setPixelColor(58, color);
  panel.setPixelColor(59, color);
  panel.setPixelColor(60, color);
  panel.setPixelColor(61, color);
  panel.show();
}

