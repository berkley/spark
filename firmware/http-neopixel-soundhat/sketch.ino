/*
Suspender side 0: 0-23
Suspender side 1: 24-47
*/

#include "application.h"
#include "neopixel.h"

// SYSTEM_MODE(MANUAL);

#define ARRAYLEN(x)  (sizeof(x) / sizeof((x)[0]))

#define PIXEL_PIN D2
#define PIXEL_COUNT 256
#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 8
#define SERIAL_WIRING 1

#define TCP_PORT 7000

//the neopixel chip type
#define PIXEL_TYPE WS2812B

#define STOP "stop"
#define SETROW "setRow"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

TCPServer server = TCPServer(TCP_PORT);
TCPClient client;

#define MAX_COLOR 255

String action;
// char action[64];
char parameters[64];
char ip[64];
char tcpstate[64];

String loopRun = STOP;
String *loopArgs = new String[20];
String *strArr = new String[20];

//audio variables
const int maxScale = 8;
const int orangeZone = 4;
const int redZone = 1;
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
unsigned int colorCount = 0;

void setup()
{
  pinMode(A0, INPUT);
  Serial.begin(9600);
//   WiFi.selectAntenna(ANT_EXTERNAL);
//   WiFi.on();
//   WiFi.connect();
//   while(!WiFi.ready())
//   {
//      Serial.print("Wifi connecting...");
//      delay(500);
//   }
  server.begin();
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  //register the action variable as a GET parameter
  // Particle.variable("action", action, STRING);
  Particle.variable("tcpstate", tcpstate, STRING);
  Particle.variable("parameters", parameters, STRING);
  Particle.variable("ip", ip, STRING);
  (String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." +  String(WiFi.localIP()[2]) + "." +  String(WiFi.localIP()[3])).toCharArray(ip, 64);
}

void loop() 
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(A0) / 4;
      // Serial.println("sample: " + String(sample));
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;
   // Serial.println("peakToPeak: " + String(peakToPeak));
   if(peakToPeak < 0)
      peakToPeak = 0;

   // map 1v p-p level to the max scale of the display
   int displayPeak = map(peakToPeak, 0, 1023, 0, maxScale);
   if(displayPeak < 0)
    displayPeak = 0;
   //upside down
   displayPeak = (maxScale - 1) - displayPeak;
   Serial.println("peakToPeak: " + String(peakToPeak) + "   displayPeak: " + String(displayPeak));

   for (int i = maxScale - 1; i >= 0; i--)
   {
      // Serial.println("i:" + String(i) + " displayPeak: " + String(displayPeak));
      if (i <= displayPeak)  
      {
         // Serial.println("off row: " + String(i));
         setRow(i, strip.Color(0,0,0));
      }
      else if(i <= redZone) //red
      {
        // Serial.println("red row: " + String(i));
        setRow(i, strip.Color(255,0,0));
      }      
      else if (i >= orangeZone) //green
      {
        // Serial.println("green row: " + String(i));
        setRow(i, Wheel(colorCount));
      }
      else if (i <= orangeZone + redZone && i > redZone) //orange
      {
        // Serial.println("orange row: " + String(i));
        setRow(i, strip.Color(255,128,0));
      }
   }

   if(colorCount >= MAX_COLOR)
    colorCount = 0;
   else
    colorCount++;

   strip.show();
}

void setRow(int row, int color)
{
  for(int i=0; i<SCREEN_WIDTH; i++)
  {
    int addr = getPixelAddress(row, i, 0);
    strip.setPixelColor(addr, color);
  }
}

int getPixelAddress(int row, int col, int wraparound)
{
    if(wraparound)
    {
        if(col >= SCREEN_WIDTH)
        {
            col = col - SCREEN_WIDTH;
        }
        else if(col <= 0)
        {
            col = SCREEN_WIDTH - col;
        }
    }
    else
    {
        if(col >= SCREEN_WIDTH || col < 0)
        {
            return -1;
        }
    }

    if(SERIAL_WIRING)
    {
        int addr;
        if(row % 2 == 0)
        {
          addr = (row * SCREEN_WIDTH) + col;
        }
        else
        {
          int offset = SCREEN_WIDTH - ((col * 2) + 1) ;
          addr = ((row * SCREEN_WIDTH) + col) + offset;
        }
        return addr;
    }
    else
    {
        return (row * SCREEN_WIDTH) + col;
    }
}

int allOff()
{
    return setAll(0,0,0);
}

int setRGB(String rgb)
{
    String r = rgb.substring(0,3);
    String g = rgb.substring(3,6);
    String b = rgb.substring(6,10);

    setAll(stringToInt(r), stringToInt(g), stringToInt(b));
    return 1;
}

int setAll(uint8_t r, uint8_t g, uint8_t b)
{
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
  return 1;
}

bool stringToBool(String s)
{
    if(s.equals("true") || s.equals("TRUE"))
        return true;
    return false;
}

int stringToInt(String s)
{
    char* str = new char[s.length() + 1];
    strcpy(str, s.c_str());
    int out = atoi(str);
    free(str);
    return out;
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

// Input a value 0 to MAX_COLOR to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  int maxVal = MAX_COLOR;
  if(WheelPos < 85) {
     return strip.Color(WheelPos * 3, maxVal - WheelPos * 3, 0);
 } else if(WheelPos < 170) {
     WheelPos -= 85;
     return strip.Color(maxVal - WheelPos * 3, 0, WheelPos * 3);
 } else {
     WheelPos -= 170;
     return strip.Color(0, WheelPos * 3, maxVal - WheelPos * 3);
 }
}

