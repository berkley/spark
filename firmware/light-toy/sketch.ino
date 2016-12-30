#include "application.h"
#include "neopixel.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

Adafruit_NeoPixel ring = Adafruit_NeoPixel(12, D0, WS2812B);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, D1, WS2812B);

void doEncoderA();
void doEncoderB();
void allOff();

int encoderA = D5;
int encoderB = D6;
int encoderButtonPin = D2;

int potPin0 = A0; //red
int potPin1 = A1; //green
int potPin2 = A2; //blue

volatile bool A_set = false;
volatile bool B_set = false;
volatile bool button_set = false;
volatile int encoderPos = 0;

int prevPos = 0;
int value = 0;

int pixelIndex = 0;

int MAX_COLOR = 255;

int ringMode = 0;
int numRingModes = 8;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  ring.begin();
  ring.show();
  ring.setBrightness(64);
  strip.begin();
  strip.show();
  strip.setBrightness(255);
  
  Serial.println("encoderPos init: " + String(encoderPos));
  
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  pinMode(encoderButtonPin, INPUT_PULLUP);
  attachInterrupt(encoderA, doEncoderA, CHANGE);
  attachInterrupt(encoderB, doEncoderB, CHANGE);
  attachInterrupt(encoderButtonPin, doEncoderButton, RISING);

  delay(1000);

  if(digitalRead(encoderButtonPin) == LOW)
  {
    Serial.println("Button pressed during startup");
    if (Particle.connected() == false) {
      Particle.connect();
    }
  }
}

void loop() {
    
    if(ringMode == 0)
    { //animate the ring and change color based on POT
      for(int i=0; i<12; i++)
        {
            ringAllOff();
            int potColor = getRGBPotVal();
            int R = analogRead(potPin0) / 16;
            int G = analogRead(potPin1) / 16;
            int B = analogRead(potPin2) / 16;
            ring.setPixelColor(i, potColor);
            ring.setPixelColor(i+1, ring.Color(R/4, G/4, B/4));
            ring.setPixelColor(i+2, ring.Color(R/8, G/8, B/8));
            ring.setPixelColor(i+3, ring.Color(R/12, G/12, B/12));
            ring.setPixelColor(i+4, ring.Color(R/16, G/16, B/16));
            ring.show();
            stripSetAll(potColor);
            delay(30);
        }
    }
    else if (ringMode == 1)
    { //change the color of a single pixel
        if (prevPos != encoderPos) {
            if(prevPos > encoderPos)
              pixelIndex++;
            else 
              pixelIndex--;
              
            prevPos = encoderPos;
            
            if(pixelIndex >=12)
              pixelIndex = 0;
            else if(pixelIndex < 0)
              pixelIndex = 11;
              
            Serial.println("pixelIndex: " + String(pixelIndex));  
            Serial.println("encoderPos: " + String(encoderPos));
            
            ringAllOff();
            ring.show();
        }
    
        int potColor = getRGBPotVal();
        ring.setPixelColor(pixelIndex, potColor);
        ring.show();
        stripSetAll(potColor);
    }
    else if (ringMode == 2)
    { //set all pixels accoring to POT
        int potColor = getRGBPotVal();
        ringSetAll(potColor);
        stripSetAll(potColor);
    }
    else if (ringMode == 3)
    {  //fade entire ring in and out on POT color
        int potColor = getRGBPotVal();
        ringSetAll(potColor);
        stripSetAll(potColor);
        for(int i=1; i<=16; i++)
        {
            int R = analogRead(potPin0) / 16;
            int G = analogRead(potPin1) / 16;
            int B = analogRead(potPin2) / 16;
            ringSetAll(ring.Color(R / i, G / i, B / i));
            delay(30);
        }
        for(int i=16; i>0; i--)
        {
            int R = analogRead(potPin0) / 16;
            int G = analogRead(potPin1) / 16;
            int B = analogRead(potPin2) / 16;
            ringSetAll(ring.Color(R / i, G / i, B / i));
            delay(30);
        }
    }
    else if(ringMode == 4)
    { //change single pixels but don't reset the whole ring
        if (prevPos != encoderPos) {
            if(prevPos > encoderPos)
              pixelIndex++;
            else 
              pixelIndex--;
              
            prevPos = encoderPos;
            
            if(pixelIndex >=12)
              pixelIndex = 0;
            else if(pixelIndex < 0)
              pixelIndex = 11;
              
            ring.show();
            delay(50);
        }
    
        int potColor = getRGBPotVal();
        ring.setPixelColor(pixelIndex, potColor);
        ring.show();
        stripSetAll(potColor);
    }
    else if(ringMode == 5)
    { //fade accoring to encoder position
        if (prevPos != encoderPos) {
            if(prevPos > encoderPos)
              pixelIndex++;
            else 
              pixelIndex--;
              
            prevPos = encoderPos;
            
            if(pixelIndex >=12)
              pixelIndex = 0;
            else if(pixelIndex < 0)
              pixelIndex = 11;
        }
        
        int R = analogRead(potPin0) / 16;
        int G = analogRead(potPin1) / 16;
        int B = analogRead(potPin2) / 16;
        if(pixelIndex == 0)
          pixelIndex++;
        Serial.println("pixelIndex (brightness): " + String(pixelIndex));
        ringSetAll(ring.Color(R / pixelIndex, G / pixelIndex, B / pixelIndex));
        
        int potColor = getRGBPotVal();
        stripSetAll(potColor);
        delay(10);
    }
    else if(ringMode == 6)
    {
        uint16_t i, j;
    
        for(j=0; j<256; j++) 
        {
            for(i=0; i<ring.numPixels(); i++) 
            {
                  ring.setPixelColor(i, Wheel((i+j) & MAX_COLOR));
                  stripSetAll(Wheel((i+j) & MAX_COLOR));
            }
            ring.show();
            delay(10);
        }
    }
    else if(ringMode == 7)
    {
        // for(int j=0; j<256; j++)
        // {
        // for(int i=0; i<12; i++)
        // {
        //     ringAllOff();
        //     int potColor = getRGBPotVal();
        //     int R = analogRead(potPin0) / 16;
        //     int G = analogRead(potPin1) / 16;
        //     int B = analogRead(potPin2) / 16;
        //     ring.setPixelColor(i, Wheel((i+j) & MAX_COLOR));
        //     ring.setPixelColor(i+1, Wheel((i+j) & MAX_COLOR));
        //     ring.setPixelColor(i+2, Wheel((i+j) & MAX_COLOR));
        //     ring.show();
        //     stripSetAll(Wheel((i+j) & MAX_COLOR));
        //     delay(30);
        // }
        // }
    }
}


int getRGBPotVal() {
    int pot0Val = analogRead(potPin0) / 16;
    int pot1Val = analogRead(potPin1) / 16;
    int pot2Val = analogRead(potPin2) / 16;
    // Serial.println("pot0Val: " + String(pot0Val));
    // Serial.println("pot1Val: " + String(pot1Val));
    // Serial.println("pot2Val: " + String(pot2Val));
    return ring.Color(pot0Val, pot1Val, pot2Val);
}

void ringAllOff()
{
    for(int i=0; i<12; i++)
    {
      ring.setPixelColor(i, ring.Color(0,0,0));
    }
}

void ringSetAll(int color)
{
    for(int i=0; i<12; i++)
    {
        ring.setPixelColor(i, color);
    }
    ring.show();
}

void stripSetAll(int color)
{
    for(int i=0; i<10; i++)
    {
        strip.setPixelColor(i, color);
    }
    strip.show();
}


void encoderButtonPushed()
{
    ringMode++;
    if(ringMode >= numRingModes)
    {
      ringMode = 0;
    }
      
    Serial.println("Mode: " + String(ringMode));
}

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int buttonState;
int lastButtonState = LOW;
void doEncoderButton() 
{
    buttonState = digitalRead(encoderButtonPin);
    if(buttonState == HIGH &&
      (millis() - lastDebounceTime) > debounceDelay) 
    {
        lastDebounceTime = millis();
        encoderButtonPushed();
    }
}

void doEncoderA(){
  
  if(digitalRead(encoderA) != A_set)
  {
    A_set = !A_set; // adjust counter + if A leads B
    Serial.println("encoderA: " + String(A_set) + "  encoderB: " + String(B_set));
    if (A_set && !B_set) 
    {
      Serial.println("encoderA-FIRE");  
      encoderPos += 1;
    }
  }
}

// Interrupt on B changing state, same as A above
void doEncoderB(){
  
  if(digitalRead(encoderB) != B_set) {
    B_set = !B_set; //  adjust counter - 1 if B leads A
    Serial.println("encoderB: " + String(B_set) + "  encoderA: " + String(A_set));   
    if(B_set && !A_set) 
    {
      Serial.println("encoderB-FIRE");
      encoderPos -= 1;
    }
  }
}

// Input a value 0 to MAX_COLOR to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
    int maxVal = MAX_COLOR;
    if(WheelPos < 85) 
    {
     return ring.Color(WheelPos * 3, maxVal - WheelPos * 3, 0);
    } 
    else if(WheelPos < 170) 
    {
     WheelPos -= 85;
     return ring.Color(maxVal - WheelPos * 3, 0, WheelPos * 3);
    } 
    else 
    {
     WheelPos -= 170;
     return ring.Color(0, WheelPos * 3, maxVal - WheelPos * 3);
    }
}