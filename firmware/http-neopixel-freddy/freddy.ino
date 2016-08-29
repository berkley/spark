/* pixel map
                0       1       2       3               
            4       5       6       7       8           
        9       10      11      12      13      14      
    15      16      17      18      19      20      21  
22      23      24      25      26      27      28      29
    30      31      32      33      34      35      36  
        37      38      39      40      41      42      
            43      44      45      46      47          
                48      49      50      51              
*/


#include "application.h"
#include "neopixel.h"

 // SYSTEM_MODE(MANUAL);

#define ARRAYLEN(x)  (sizeof(x) / sizeof((x)[0]))

//the pin your spark is using to control neopixels
#define PIXEL_PIN D2
//the number of pixels you are controlling
// #define PIXEL_COUNT 467 //giggle
#define PIXEL_COUNT 150 //colossus
// #define PIXEL_COUNT 104 //FREDDY
#define EYE_PIXEL_COUNT 52

#define TCP_PORT 7000

//the neopixel chip type
#define PIXEL_TYPE WS2812B

#define STOP "stop"
#define HEARTEYES "hearteyes"
#define SPIRALEYES "spiraleyes"
#define SPARKLEEYES "sparkleeyes"
#define STATICHEARTEYES "statichearteyes"
#define RAINBOWEYES "rainboweyes"
#define WAKEUPEYES "wakeupeyes"
#define GLOWEYES "gloweyes"
#define ALLOFF "alloff"
#define FIRESPARKLE "firesparkle"
#define FIREHEART "fireheart"
#define FIRESPIRAL "firespiral"
#define RANDOM "random"
#define KEITHEYES "keitheyes"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

TCPServer server = TCPServer(TCP_PORT);
TCPClient client;

#define MAX_COLOR 255

char action[64];
char parameters[64];
char ip[64];
char tcpstate[64];

String loopRun = RANDOM;
String *loopArgs = new String[20];
String *strArr = new String[20];

void setup()
{
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
  strip.setBrightness(128);
  
  Serial.println("Serial begin");
  
  pinMode(D5, OUTPUT);
  pinMode(D4, OUTPUT);
  
  //register the action variable as a GET parameter
  Spark.variable("action", action, STRING);
  Spark.variable("tcpstate", tcpstate, STRING);
  Spark.variable("parameters", parameters, STRING);
  Spark.variable("ip", ip, STRING);
  (String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." +  String(WiFi.localIP()[2]) + "." +  String(WiFi.localIP()[3])).toCharArray(ip, 64);
  strip.setPixelColor(0, strip.Color(0, 50, 0));
  strip.show();
  delay(100);
  strip.setPixelColor(0, strip.Color(50, 0, 0));
  strip.show();
  delay(100);
  strip.setPixelColor(0, strip.Color(0, 0, 50));
  strip.show();
  delay(100);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
}

void loop()
{
    // Serial.println("IP:" + String(ip));
    //TCPServer/Client connections
    if (client.connected()) {
        String("CONNECTED").toCharArray(tcpstate, 64);
        String dStr = "";
        char data;
        while (client.available()) 
        {
            data = client.read();
            dStr += String(data);
        }
        client.println("OK:" + dStr);
        client.flush();
        dStr.toCharArray(action, 64);
        loopRun = action;
        client.stop();
    } 
    else 
    {
        // if no client is yet connected, check for a new connection
        String("DISCONNECTED").toCharArray(tcpstate, 64);
        client = server.available();
        if(client.connected())
            return;
    }
    
    //////////command processing////////////////////
      
    if(loopRun.equals(STOP))
    {
        delay(100);
    }
    else if(loopRun.equals(ALLOFF))
    {
        allOff();
        loopRun = STOP;
    }
    else if(loopRun.equals(HEARTEYES))
    {
        heartEyes();
    }
    else if(loopRun.equals(SPIRALEYES))
    {
        spiralEyes();
    }
    else if(loopRun.equals(SPARKLEEYES))
    {
        sparkleEyes();
    }
    else if(loopRun.equals(STATICHEARTEYES))
    {
        staticHeartEyes();
    }
    else if(loopRun.equals(RAINBOWEYES))
    {
        rainbowEyes();
    }
    else if(loopRun.equals(GLOWEYES))
    {
        glowEyes();
    }
    else if(loopRun.equals(WAKEUPEYES))
    {
        wakeUpEyes();
    }
    else if(loopRun.equals(FIRESPARKLE))
    {
        fireSparkle();
    }
    else if(loopRun.equals(FIREHEART))
    {
        fireHeart();
    }
    else if(loopRun.equals(FIRESPIRAL))
    {
        fireSpiral();
    }
    else if(loopRun.equals(RANDOM))
    {
        random();
    }
    else if(loopRun.equals(KEITHEYES))
    {
        keithEyes();
    }
    else
    {
        String("INVALID_ACTION").toCharArray(action, 64);
        loopRun = STOP;
    }
}

int keithEyes()
{
    uint32_t c1 = strip.Color(160, 160, 160);
    uint32_t c2 = strip.Color(250, 0, 0);
    uint32_t c3 = strip.Color(0, 0, 0);
    
    int farLeftColor1[] = {1, 2, 3, 6, 5, 4, 12, 13, 14, 17, 16, 15, 26, 27, 28, 29, 32, 31, 30, 40, 41, 42, 45, 44, 43, 49, 50, 51};
    int farLeftColor2[] = {0, 8, 7, 9, 11, 21, 18, 22, 25, 36, 33, 37, 39, 47, 46, 48};
    int farLeftColor3[] = {10, 20, 19, 23, 24, 35, 34, 38};
    
    int leftColor1[] = {2, 3, 5, 4, 13, 14, 16, 15, 22, 27, 28, 29, 31, 30, 41, 42, 44, 43, 50, 51};
    int leftColor2[] = {0, 1, 8, 7, 6, 9, 12, 21, 17, 23, 26, 36, 32, 37, 40, 47, 46, 45, 48, 49};
    int leftColor3[] = {10, 11, 20, 19, 18, 24, 25, 35, 34, 33, 38, 39};
    
    int centerColor1[] = {0, 3, 8, 4, 9, 14, 21, 15, 22, 23, 28, 29, 36, 30, 37, 42, 47, 43, 48, 51};
    int centerColor2[] = {1, 2, 7, 6, 5, 10, 13, 20, 16, 24, 27, 35, 31, 38, 41, 46, 45, 44, 49, 50};
    int centerColor3[] = {11, 12, 19, 18, 17, 25, 26, 34, 33, 32, 39, 40};
    
    int rightColor1[] = {0, 1, 8, 7, 9, 10, 21, 20, 22, 23, 24, 29, 36, 35, 37, 38, 47, 46, 48, 49};
    int rightColor2[] = {2, 3, 6, 5, 4, 11, 14, 19, 15, 25, 28, 34, 30, 39, 42, 45, 44, 43, 50, 51};
    int rightColor3[] = {12, 13, 18, 17, 16, 26, 27, 33, 32, 31, 40, 41};

    int farRightColor1[] = {0, 1, 2, 8, 7, 6, 9, 10, 11, 21, 20, 19, 22, 23, 24, 25, 36, 35, 34, 37, 38, 39, 47, 46, 45, 48, 49, 50};
    int farRightColor2[] = {3, 5, 4, 12, 14, 18, 15, 26, 29, 33, 30, 40, 42, 44, 43, 51};
    int farRightColor3[] = {13, 17, 16, 27, 28, 32, 31, 41};

    int downRightColor1[] = {0, 1, 2, 3, 8, 7, 6, 9, 10, 11, 21, 20, 19, 22, 23, 24, 36, 35, 34, 37, 38, 47, 46, 48, 49};
    int downRightColor2[] = {5, 4, 12, 13, 14, 18, 15, 25, 29, 33, 30, 39, 45, 50, 51};
    int downRightColor3[] = {17, 16, 26, 27, 28, 32, 31, 40, 41, 42, 44, 43};

    int downMidRightColor1[] = {0, 1, 2, 3, 8, 7, 4, 9, 10, 14, 21, 20, 15, 22, 23, 29, 36, 35, 30, 37, 47, 48};
    int downMidRightColor2[] = {6, 5, 11, 12, 13, 19, 16, 24, 28, 34, 31, 38, 42, 46, 43, 49, 50, 51};
    int downMidRightColor3[] = {18, 17, 25, 26, 27, 33, 32, 39, 40, 41, 45, 44};

    int downMidLeftColor1[] = {0, 1, 2, 3, 8, 5, 4, 9, 13, 14, 21, 16, 15, 22, 28, 29, 36, 31, 30, 42, 43, 51};
    int downMidLeftColor2[] = {7, 6, 10, 11, 12, 20, 17, 23, 27, 35, 32, 37, 41, 47, 44, 48, 49, 50};
    int downMidLeftColor3[] = {19, 18, 24, 25, 26, 34, 33, 38, 39, 40, 46, 45};

    int downLeftColor1[] = {0, 1, 2, 3, 6, 5, 4, 12, 13, 14, 17, 16, 15, 27, 28, 29, 32, 31, 30, 41, 42, 44, 43, 50, 51};
    int downLeftColor2[] = {8, 7, 9, 10, 11, 21, 18, 22, 26, 36, 33, 40, 45, 48, 49};
    int downLeftColor3[] = {20, 19, 23, 24, 25, 35, 34, 37, 38, 39, 47, 46};

    int blink1Color1[] = {4, 4, 9, 14, 21, 15, 22, 23, 28, 29, 36, 30, 37, 42, 47, 43, 48, 51};
    int blink1Color2[] = {5, 6, 5, 10, 13, 20, 16, 24, 27, 35, 31, 38, 41, 46, 45, 44, 49, 50};
    int blink1Color3[] = {0, 1, 2, 3, 11, 12, 19, 18, 17, 25, 26, 34, 33, 32, 39, 40};

    int blink2Color1[] = {9, 14, 21, 15, 22, 23, 28, 29, 36, 30, 37, 42, 47, 43, 48, 51};
    int blink2Color2[] = {10, 13, 20, 16, 24, 27, 35, 31, 38, 41, 46, 45, 44, 49, 50};
    int blink2Color3[] = {0, 1, 2, 3, 8, 7, 6, 5, 4, 11, 12, 19, 18, 17, 25, 26, 34, 33, 32, 39, 40};

    int blink3Color1[] = {15, 15, 22, 23, 28, 29, 36, 30, 37, 42, 47, 43, 48, 51};
    int blink3Color2[] = {16, 16, 24, 27, 35, 31, 38, 41, 46, 45, 44, 49, 50};
    int blink3Color3[] = {0, 1, 2, 3, 8, 7, 6, 5, 4, 9, 10, 11, 12, 13, 14, 19, 18, 17, 25, 26, 34, 33, 32, 39, 40};

    int blink4Color1[] = {22, 29, 36, 30, 37, 42, 47, 43, 48, 51};
    int blink4Color2[] = {31, 31, 38, 41, 46, 45, 44, 49, 50};
    int blink4Color3[] = {0, 1, 2, 3, 8, 7, 6, 5, 4, 9, 10, 11, 12, 13, 14, 21, 20, 19, 18, 17, 16, 15, 23, 24, 25, 26, 27, 28, 34, 33, 32, 39, 40};

    int blink5Color1[] = {37, 42, 47, 43, 48, 51};
    int blink5Color2[] = {49, 50};
    int blink5Color3[] = {0, 1, 2, 3, 8, 7, 6, 5, 4, 9, 10, 11, 12, 13, 14, 21, 20, 19, 18, 17, 16, 15, 22, 23, 24, 25, 26, 27, 28, 29, 36, 35, 32};

    int blink6Color1[] = {};
    int blink6Color2[] = {};
    int blink6Color3[] = {0, 1, 2, 3, 8, 7, 6, 5, 4, 9, 10, 11, 12, 13, 14, 21, 20, 19, 18, 17, 16, 15, 22, 23, 24, 25, 26, 27, 28, 29, 36, 35, 34, 33, 32, 31, 30, 37, 38, 39, 40, 41, 42, 47, 46, 45, 44, 43, 48, 49, 50, 51};

    setPixels(leftColor1, c1, ARRAYLEN(leftColor1));
    setPixels(leftColor2, c2, ARRAYLEN(leftColor2));
    setPixels(leftColor3, c3, ARRAYLEN(leftColor3));
    strip.show();
    delay(250); //2
    
    setPixels(farLeftColor1, c1, ARRAYLEN(farLeftColor1));
    setPixels(farLeftColor2, c2, ARRAYLEN(farLeftColor2));
    setPixels(farLeftColor3, c3, ARRAYLEN(farLeftColor3));
    strip.show();
    delay(1000); //3
    
    setPixels(leftColor1, c1, ARRAYLEN(leftColor1));
    setPixels(leftColor2, c2, ARRAYLEN(leftColor2));
    setPixels(leftColor3, c3, ARRAYLEN(leftColor3));
    strip.show();
    delay(250); //4
    
    setPixels(centerColor1, c1, ARRAYLEN(centerColor1));
    setPixels(centerColor2, c2, ARRAYLEN(centerColor2));
    setPixels(centerColor3, c3, ARRAYLEN(centerColor3));
    strip.show();
    delay(250); //5
    
    setPixels(rightColor1, c1, ARRAYLEN(rightColor1));
    setPixels(rightColor2, c2, ARRAYLEN(rightColor2));
    setPixels(rightColor3, c3, ARRAYLEN(rightColor3));
    strip.show();
    delay(250); //6
    
    setPixels(farRightColor1, c1, ARRAYLEN(farRightColor1));
    setPixels(farRightColor2, c2, ARRAYLEN(farRightColor2));
    setPixels(farRightColor3, c3, ARRAYLEN(farRightColor3));
    strip.show();
    delay(1000); //7
    
    setPixels(rightColor2, c2, ARRAYLEN(rightColor2));
    setPixels(rightColor1, c1, ARRAYLEN(rightColor1));
    setPixels(rightColor3, c3, ARRAYLEN(rightColor3));
    strip.show();
    delay(250); //8
    
    setPixels(centerColor1, c1, ARRAYLEN(centerColor1));
    setPixels(centerColor2, c2, ARRAYLEN(centerColor2));
    setPixels(centerColor3, c3, ARRAYLEN(centerColor3));
    strip.show();
    delay(1000); //9
    
    setPixels(blink1Color1, c1, ARRAYLEN(blink1Color1));
    setPixels(blink1Color2, c2, ARRAYLEN(blink1Color2));
    setPixels(blink1Color3, c3, ARRAYLEN(blink1Color3));
    strip.show();
    delay(50); //10
    
    setPixels(blink2Color1, c1, ARRAYLEN(blink2Color1));
    setPixels(blink2Color2, c2, ARRAYLEN(blink2Color2));
    setPixels(blink2Color3, c3, ARRAYLEN(blink2Color3));
    strip.show();
    delay(50); //11
    
    setPixels(blink3Color1, c1, ARRAYLEN(blink3Color1));
    setPixels(blink3Color2, c2, ARRAYLEN(blink3Color2));
    setPixels(blink3Color3, c3, ARRAYLEN(blink3Color3));
    strip.show();
    delay(50); //12
    
    setPixels(blink4Color1, c1, ARRAYLEN(blink4Color1));
    setPixels(blink4Color2, c2, ARRAYLEN(blink4Color2));
    setPixels(blink4Color3, c3, ARRAYLEN(blink4Color3));
    strip.show(); 
    delay(50); //13
    
    setPixels(blink5Color1, c1, ARRAYLEN(blink5Color1));
    setPixels(blink5Color2, c2, ARRAYLEN(blink5Color2));
    setPixels(blink5Color3, c3, ARRAYLEN(blink5Color3));
    strip.show();
    delay(50); //14
    
    setPixels(blink6Color1, c1, ARRAYLEN(blink6Color1));
    setPixels(blink6Color2, c2, ARRAYLEN(blink6Color2));
    setPixels(blink6Color3, c3, ARRAYLEN(blink6Color3));
    strip.show();
    delay(150); //15
    
    setPixels(blink5Color1, c1, ARRAYLEN(blink5Color1));
    setPixels(blink5Color2, c2, ARRAYLEN(blink5Color2));
    setPixels(blink5Color3, c3, ARRAYLEN(blink5Color3));
    strip.show();
    delay(50); //16
    
    setPixels(blink4Color1, c1, ARRAYLEN(blink4Color1));
    setPixels(blink4Color2, c2, ARRAYLEN(blink4Color2));
    setPixels(blink4Color3, c3, ARRAYLEN(blink4Color3));
    strip.show();
    delay(50); //17
    
    setPixels(blink3Color1, c1, ARRAYLEN(blink3Color1));
    setPixels(blink3Color2, c2, ARRAYLEN(blink3Color2));
    setPixels(blink3Color3, c3, ARRAYLEN(blink3Color3));
    strip.show();
    delay(50); //18
    
    setPixels(blink2Color1, c1, ARRAYLEN(blink2Color1));
    setPixels(blink2Color2, c2, ARRAYLEN(blink2Color2));
    setPixels(blink2Color3, c3, ARRAYLEN(blink2Color3));
    strip.show();
    delay(50); //19
    
    setPixels(blink1Color1, c1, ARRAYLEN(blink1Color1));
    setPixels(blink1Color2, c2, ARRAYLEN(blink1Color2));
    setPixels(blink1Color3, c3, ARRAYLEN(blink1Color3));
    strip.show();
    delay(50); //20
    
    setPixels(centerColor1, c1, ARRAYLEN(centerColor1));
    setPixels(centerColor2, c2, ARRAYLEN(centerColor2));
    setPixels(centerColor3, c3, ARRAYLEN(centerColor3));
    strip.show();
    delay(1000); //21
    
    setPixels(rightColor1, c1, ARRAYLEN(rightColor1));
    setPixels(rightColor2, c2, ARRAYLEN(rightColor2));
    setPixels(rightColor3, c3, ARRAYLEN(rightColor3));
    strip.show();
    delay(250); //22
    
    setPixels(downRightColor1, c1, ARRAYLEN(downRightColor1));
    setPixels(downRightColor2, c2, ARRAYLEN(downRightColor2));
    setPixels(downRightColor3, c3, ARRAYLEN(downRightColor3));
    strip.show();
    delay(1000); //23
    
    setPixels(downMidRightColor1, c1, ARRAYLEN(downMidRightColor1));
    setPixels(downMidRightColor2, c2, ARRAYLEN(downMidRightColor2));
    setPixels(downMidRightColor3, c3, ARRAYLEN(downMidRightColor3));
    strip.show();
    delay(250); //24
    
    setPixels(downMidLeftColor1, c1, ARRAYLEN(downMidLeftColor1));
    setPixels(downMidLeftColor2, c2, ARRAYLEN(downMidLeftColor2));
    setPixels(downMidLeftColor3, c3, ARRAYLEN(downMidLeftColor3));
    strip.show();
    delay(250); //25
    
    setPixels(downLeftColor1, c1, ARRAYLEN(downLeftColor1));
    setPixels(downLeftColor2, c2, ARRAYLEN(downLeftColor2));
    setPixels(downLeftColor3, c3, ARRAYLEN(downLeftColor3));
    strip.show();
    delay(1000); //26
    
    setPixels(downMidLeftColor1, c1, ARRAYLEN(downMidLeftColor1));
    setPixels(downMidLeftColor2, c2, ARRAYLEN(downMidLeftColor2));
    setPixels(downMidLeftColor3, c3, ARRAYLEN(downMidLeftColor3));
    strip.show();
    delay(250); //27
    
    setPixels(downMidRightColor1, c1, ARRAYLEN(downMidRightColor1));
    setPixels(downMidRightColor2, c2, ARRAYLEN(downMidRightColor2));
    setPixels(downMidRightColor3, c3, ARRAYLEN(downMidRightColor3));
    strip.show();
    delay(250); //28
    
    setPixels(downRightColor1, c1, ARRAYLEN(downRightColor1));
    setPixels(downRightColor2, c2, ARRAYLEN(downRightColor2));
    setPixels(downRightColor3, c3, ARRAYLEN(downRightColor3));
    strip.show();
    delay(1000); //29
    
    setPixels(downMidLeftColor1, c1, ARRAYLEN(downMidLeftColor1));
    setPixels(downMidLeftColor2, c2, ARRAYLEN(downMidLeftColor2));
    setPixels(downMidLeftColor3, c3, ARRAYLEN(downMidLeftColor3));
    strip.show();
    delay(250); //30
    
    setPixels(downMidRightColor1, c1, ARRAYLEN(downMidRightColor1));
    setPixels(downMidRightColor2, c2, ARRAYLEN(downMidRightColor2));
    setPixels(downMidRightColor3, c3, ARRAYLEN(downMidRightColor3));
    strip.show();
    delay(250); //31
    
    setPixels(downMidLeftColor1, c1, ARRAYLEN(downMidLeftColor1));
    setPixels(downMidLeftColor2, c2, ARRAYLEN(downMidLeftColor2));
    setPixels(downMidLeftColor3, c3, ARRAYLEN(downMidLeftColor3));
    strip.show();
    delay(1000); //32
    
    setPixels(downMidRightColor1, c1, ARRAYLEN(downMidRightColor1));
    setPixels(downMidRightColor2, c2, ARRAYLEN(downMidRightColor2));
    setPixels(downMidRightColor3, c3, ARRAYLEN(downMidRightColor3));
    strip.show();
    delay(250); //33
    
    setPixels(downRightColor1, c1, ARRAYLEN(downRightColor1));
    setPixels(downRightColor2, c2, ARRAYLEN(downRightColor2));
    setPixels(downRightColor3, c3, ARRAYLEN(downRightColor3));
    strip.show();
    delay(250); //34
    
    setPixels(rightColor1, c1, ARRAYLEN(rightColor1));
    setPixels(rightColor2, c2, ARRAYLEN(rightColor2));
    setPixels(rightColor3, c3, ARRAYLEN(rightColor3));
    strip.show();
    delay(1000); //35
    
    setPixels(centerColor1, c1, ARRAYLEN(centerColor1));
    setPixels(centerColor2, c2, ARRAYLEN(centerColor2));
    setPixels(centerColor3, c3, ARRAYLEN(centerColor3));
    strip.show();
    delay(250); //36
    
    setPixels(downMidRightColor1, c1, ARRAYLEN(downMidRightColor1));
    setPixels(downMidRightColor2, c2, ARRAYLEN(downMidRightColor2));
    setPixels(downMidRightColor3, c3, ARRAYLEN(downMidRightColor3));
    strip.show();
    delay(1000); //37
    
    setPixels(blink1Color1, c1, ARRAYLEN(blink1Color1));
    setPixels(blink1Color2, c2, ARRAYLEN(blink1Color2));
    setPixels(blink1Color3, c3, ARRAYLEN(blink1Color3));
    strip.show();
    delay(50); //38
    
    setPixels(blink2Color1, c1, ARRAYLEN(blink2Color1));
    setPixels(blink2Color2, c2, ARRAYLEN(blink2Color2));
    setPixels(blink2Color3, c3, ARRAYLEN(blink2Color3));
    strip.show();
    delay(50); //39
    
    setPixels(blink3Color1, c1, ARRAYLEN(blink3Color1));
    setPixels(blink3Color2, c2, ARRAYLEN(blink3Color2));
    setPixels(blink3Color3, c3, ARRAYLEN(blink3Color3));
    strip.show();
    delay(50); //40
    
    setPixels(blink4Color1, c1, ARRAYLEN(blink4Color1));
    setPixels(blink4Color2, c2, ARRAYLEN(blink4Color2));
    setPixels(blink4Color3, c3, ARRAYLEN(blink4Color3));
    strip.show();
    delay(50); //41
    
    setPixels(blink5Color1, c1, ARRAYLEN(blink5Color1));
    setPixels(blink5Color2, c2, ARRAYLEN(blink5Color2));
    setPixels(blink5Color3, c3, ARRAYLEN(blink5Color3));
    strip.show();
    delay(50); //42
    
    setPixels(blink6Color1, c1, ARRAYLEN(blink6Color1));
    setPixels(blink6Color2, c2, ARRAYLEN(blink6Color2));
    setPixels(blink6Color3, c3, ARRAYLEN(blink6Color3));
    strip.show();
    delay(50); //43
    
    setPixels(blink5Color1, c1, ARRAYLEN(blink5Color1));
    setPixels(blink5Color2, c2, ARRAYLEN(blink5Color2));
    setPixels(blink5Color3, c3, ARRAYLEN(blink5Color3));
    strip.show();
    delay(50); //44
    
    setPixels(blink4Color1, c1, ARRAYLEN(blink4Color1));
    setPixels(blink4Color2, c2, ARRAYLEN(blink4Color2));
    setPixels(blink4Color3, c3, ARRAYLEN(blink4Color3));
    strip.show();
    delay(50); //45
    
    setPixels(blink3Color1, c1, ARRAYLEN(blink3Color1));
    setPixels(blink3Color2, c2, ARRAYLEN(blink3Color2));
    setPixels(blink3Color3, c3, ARRAYLEN(blink3Color3));
    strip.show();
    delay(50); //46
    
    setPixels(blink2Color1, c1, ARRAYLEN(blink2Color1));
    setPixels(blink2Color2, c2, ARRAYLEN(blink2Color2));
    setPixels(blink2Color3, c3, ARRAYLEN(blink2Color3));
    strip.show();
    delay(50); //47
    
    setPixels(blink1Color1, c1, ARRAYLEN(blink1Color1));
    setPixels(blink1Color2, c2, ARRAYLEN(blink1Color2));
    setPixels(blink1Color3, c3, ARRAYLEN(blink1Color3));
    strip.show();
    delay(50); //48
    
    setPixels(centerColor1, c1, ARRAYLEN(centerColor1));
    setPixels(centerColor2, c2, ARRAYLEN(centerColor2));
    setPixels(centerColor3, c3, ARRAYLEN(centerColor3));
    strip.show();
    delay(3000); //49
    
    setPixels(blink1Color1, c1, ARRAYLEN(blink1Color1));
    setPixels(blink1Color2, c2, ARRAYLEN(blink1Color2));
    setPixels(blink1Color3, c3, ARRAYLEN(blink1Color3));
    strip.show();
    delay(50); //50
    
    setPixels(blink2Color1, c1, ARRAYLEN(blink2Color1));
    setPixels(blink2Color2, c2, ARRAYLEN(blink2Color2));
    setPixels(blink2Color3, c3, ARRAYLEN(blink2Color3));
    strip.show();
    delay(50); //51
    
    setPixels(blink3Color1, c1, ARRAYLEN(blink3Color1));
    setPixels(blink3Color2, c2, ARRAYLEN(blink3Color2));
    setPixels(blink3Color3, c3, ARRAYLEN(blink3Color3));
    strip.show();
    delay(50); //52
    
    setPixels(blink4Color1, c1, ARRAYLEN(blink4Color1));
    setPixels(blink4Color2, c2, ARRAYLEN(blink4Color2));
    setPixels(blink4Color3, c3, ARRAYLEN(blink4Color3));
    strip.show();
    delay(50); //53
    
    setPixels(blink5Color1, c1, ARRAYLEN(blink5Color1));
    setPixels(blink5Color2, c2, ARRAYLEN(blink5Color2));
    setPixels(blink5Color3, c3, ARRAYLEN(blink5Color3));
    strip.show();
    delay(50); //54
    
    setPixels(blink6Color1, c1, ARRAYLEN(blink6Color1));
    setPixels(blink6Color2, c2, ARRAYLEN(blink6Color2));
    setPixels(blink6Color3, c3, ARRAYLEN(blink6Color3));
    strip.show();
    delay(50); //55
    
    setPixels(blink5Color1, c1, ARRAYLEN(blink5Color1));
    setPixels(blink5Color2, c2, ARRAYLEN(blink5Color2));
    setPixels(blink5Color3, c3, ARRAYLEN(blink5Color3));
    strip.show();
    delay(50); //56
    
    setPixels(blink4Color1, c1, ARRAYLEN(blink4Color1));
    setPixels(blink4Color2, c2, ARRAYLEN(blink4Color2));
    setPixels(blink4Color3, c3, ARRAYLEN(blink4Color3));
    strip.show();
    delay(50); //57
    
    setPixels(blink3Color1, c1, ARRAYLEN(blink3Color1));
    setPixels(blink3Color2, c2, ARRAYLEN(blink3Color2));
    setPixels(blink3Color3, c3, ARRAYLEN(blink3Color3));
    strip.show();
    delay(50); //58
    
    setPixels(blink2Color1, c1, ARRAYLEN(blink2Color1));
    setPixels(blink2Color2, c2, ARRAYLEN(blink2Color2));
    setPixels(blink2Color3, c3, ARRAYLEN(blink2Color3));
    strip.show();
    delay(50); //59
    
    setPixels(blink1Color1, c1, ARRAYLEN(blink1Color1));
    setPixels(blink1Color2, c2, ARRAYLEN(blink1Color2));
    setPixels(blink1Color3, c3, ARRAYLEN(blink1Color3));
    strip.show();
    delay(50); //60
    
    setPixels(centerColor1, c1, ARRAYLEN(centerColor1));
    setPixels(centerColor2, c2, ARRAYLEN(centerColor2));
    setPixels(centerColor3, c3, ARRAYLEN(centerColor3));
    strip.show();
    delay(1000); //61
}

void setPixels(int pix[], uint32_t color, int arrlen)
{
    for(int i=0; i<arrlen; i++)
    {
        // bool rev = false;
        // if(pix[i]>=4 && pix[i] <=8 ||
        //   pix[i]>=15 && pix[i] <=21 ||
        //   pix[i]>=30 && pix[i]<=36 ||
        //   pix[i]>=43 && pix[i]<=47)
        // {
        //     rev = true;
        // }
        // if(rev)
        // {
            
        // }
        // else
        // {
        //     strip.setPixelColor(pix[i], color);
        // }
        
        strip.setPixelColor(pix[i], color);
        strip.setPixelColor(pix[i] + EYE_PIXEL_COUNT, color);
    }
}


int random()
{
    /*
    0#define HEARTEYES "hearteyes"
    1#define SPIRALEYES "spiraleyes"
    2#define SPARKLEEYES "sparkleeyes"
    3#define STATICHEARTEYES "statichearteyes"
    4#define RAINBOWEYES "rainboweyes"
    5#define WAKEUPEYES "wakeupeyes"
    6#define GLOWEYES "gloweyes"*/
    int r = random(6);
    allOff();
    switch (r)
    {
        case 0:
            heartEyes();
            break;
        case 1:
            spiralEyes();
            break;
        case 2:
            sparkleEyes();
            break;
        case 3: 
            staticHeartEyes();
            break;
        case 4:
            rainbowEyes();
            break;
        case 5: 
            wakeUpFreddy();
            break;
        case 6:
            glowEyes();
            break;
        default:
            staticHeartEyes();
            break;
    }
}

void HSIOn()
{
    digitalWrite(D4, HIGH); //turn on hot surface igniter
}

void HSIOff()
{
    digitalWrite(D4, LOW); //turn off hot surface igniter
}

void gasOn()
{
    digitalWrite(D5, HIGH);
}

void gasOff()
{
    digitalWrite(D5, LOW);
}

int fireHeart()
{
    allOff();
    HSIOn();
    drawHeartEye(35, 0, 60);
    strip.show();
    delay(500);
    drawHeartEye(0, 255, 0);
    strip.show();
    delay(300);
    drawHeartEye(35, 0, 60);
    strip.show();
    delay(300);
    drawHeartEye(0, 255, 0);
    strip.show();
    delay(300);
    drawHeartEye(35, 0, 60);
    strip.show();
    delay(300);
    drawHeartEye(0, 255, 0);
    strip.show();
    delay(300);
    drawHeartEye(35, 0, 60);
    strip.show();
    delay(500);
    gasOn();
    drawHeartEye(35, 0, 60);
    strip.show();
    delay(1000);
    gasOff();
    HSIOff();
    loopRun = STOP;
}

int fireSpiral()
{
    HSIOn();
    for(int i=0; i<10; i++)
        spiralFreddy(random(128), random(128), random(255), random(50), random(50), random(50));
        
    gasOn();
    delay(200);
    gasOff();
    delay(200);
    gasOn();
    delay(200);
    gasOff();
    delay(200);
    gasOn();
    delay(200);
    gasOff();
    HSIOff();
    blink(50);
    narrowEyes(50);
    loopRun = STOP;
}

int fireSparkle()
{
    HSIOn();
    
    fadeColor(255, 0, 0, 0, 0, 0, 50, 3000);
    
    //turn on the gas
    gasOn();
    
    for(int j=0; j<30; j++)
    {
        int cnt = 10;
        int pixNums[cnt];
        for(int i=0; i<cnt; i++)
        {
            int pix = random(strip.numPixels());
            pixNums[i] = pix;
            strip.setPixelColor(pix, strip.Color(random(255),random(255),random(255)));
        }
        strip.show();
        delay(30);
        for(int i=0; i<cnt; i++)
        {
            int pix = pixNums[i];
            strip.setPixelColor(pix, strip.Color(255,0,0));
        }
        strip.show();
        delay(30);
    }
    
    HSIOff();
    gasOff();
    
    delay(100);
    blink(100);
    narrowEyes(100);

    // loopRun = RANDOM;
    loopRun = STOP;
}

int heartEyes()
{
    drawHeartEye(35, 0, 60);
    strip.show();
    delay(100);
    return 1;
}

int spiralEyes()
{
    for(int i=0; i<50; i++)
        spiralFreddy(random(128), random(128), random(255), random(50), random(50), random(50));
    return 1;
}

int sparkleEyes()
{
    sparkleFreddy();
    delay(100);
    return 1;
}

int staticHeartEyes()
{
    heartEyeFreddy(75, 0, 130);
    delay(100);
    return 1;
}

int rainbowEyes()
{
    rainbow(20);
    delay(100);
    return 1;
}

int wakeUpEyes()
{
    wakeUpFreddy();
    delay(1000);
    return 1;
}

int glowEyes()
{
    glowEyeFreddy(20);
    delay(1000);
    return 1;
}

int allOff()
{
    return setAll(0,0,0);
}


///////////////////////////////////////IMP///////////////////////////////////////

int setRow(int row, int eye, uint8_t r, uint8_t g, uint8_t b)
{
    int offset = 0;
    if(eye != 0)
        offset = EYE_PIXEL_COUNT;
        
    if(row == 0)
    { //4 pix
        for(int i=0; i<4; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    else if(row == 1)
    { //5 pix
        for(int i=4; i<9; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    else if(row == 2)
    { //6 pix
        for(int i=9; i<15; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    else if(row == 3)
    { //7 pix
        for(int i=15; i<22; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    else if(row == 4)
    { //8 pix, middle row
        for(int i=22; i<30; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    else if(row == 5)
    { //7 pix
        for(int i=30; i<37; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    else if(row == 6)
    { //6 pix
        for(int i=37; i<43; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    else if(row == 7)
    { //5 pix
        for(int i=43; i<48; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    else if(row == 8)
    { //4 pix
        for(int i=48; i<52; i++)
            strip.setPixelColor(i + offset, strip.Color(r, g, b));
    }
    strip.show();
}

int closeEyes(int del)
{
    // drawPupil(0, 0, 255, 0);  drawPupil(1, 0, 255, 0);
    setRow(0, 0, 255, 0, 0);  setRow(0, 1, 255, 0, 0);
    setRow(8, 0, 255, 0, 0);  setRow(8, 1, 255, 0, 0);
    delay(del);
    // drawPupil(0, 0, 255, 0);  drawPupil(1, 0, 255, 0);
    setRow(1, 0, 255, 0, 0);  setRow(1, 1, 255, 0, 0);
    setRow(7, 0, 255, 0, 0);  setRow(7, 1, 255, 0, 0);
    delay(del);
    // drawPupil(0, 0, 255, 0);  drawPupil(1, 0, 255, 0);
    setRow(2, 0, 255, 0, 0);  setRow(2, 1, 255, 0, 0);
    setRow(6, 0, 255, 0, 0);  setRow(6, 1, 255, 0, 0);
    delay(del);
    // drawPupil(0, 0, 255, 0);  drawPupil(1, 0, 255, 0);
    setRow(3, 0, 255, 0, 0);  setRow(3, 1, 255, 0, 0);
    setRow(5, 0, 255, 0, 0);  setRow(5, 1, 255, 0, 0);
    delay(del);
    // drawPupil(0, 0, 255, 0);  drawPupil(1, 0, 255, 0);
    setRow(4, 0, 255, 0, 0);  setRow(4, 1, 255, 0, 0);
}

int openEyes(int del)
{
    setRow(4, 0, 0, 0, 0);  setRow(4, 1, 0, 0, 0);
    drawPupilsRow(2, 0, 255, 0); 
    strip.show();
    
    delay(del);
    setRow(3, 0, 0, 0, 0);  setRow(3, 1, 0, 0, 0);
    setRow(5, 0, 0, 0, 0);  setRow(5, 1, 0, 0, 0);
    drawPupilsRow(1, 0, 255, 0); 
    drawPupilsRow(3, 0, 255, 0); 
    strip.show();
    
    delay(del);
    setRow(2, 0, 0, 0, 0);  setRow(2, 1, 0, 0, 0);
    setRow(6, 0, 0, 0, 0);  setRow(6, 1, 0, 0, 0);
    drawPupilsRow(0, 0, 255, 0); 
    drawPupilsRow(4, 0, 255, 0); 
    strip.show();
    
    delay(del);
    setRow(1, 0, 0, 0, 0);  setRow(1, 1, 0, 0, 0);
    setRow(7, 0, 0, 0, 0);  setRow(7, 1, 0, 0, 0);
    delay(del);
    setRow(0, 0, 0, 0, 0);  setRow(0, 1, 0, 0, 0);
    setRow(8, 0, 0, 0, 0);  setRow(8, 1, 0, 0, 0);
    delay(del);
}

int narrowEyes(int del)
{
    closeEyes(del);
    
    setRow(4, 0, 0, 0, 0);  setRow(4, 1, 0, 0, 0);
    drawPupilsRow(2, 0, 255, 0); 
    strip.show();
    
    delay(del);
    setRow(3, 0, 0, 0, 0);  setRow(3, 1, 0, 0, 0);
    setRow(5, 0, 0, 0, 0);  setRow(5, 1, 0, 0, 0);
    drawPupilsRow(1, 0, 255, 0); 
    drawPupilsRow(3, 0, 255, 0); 
    strip.show();
}


int drawPupilRow(int eye, int row, uint8_t r, uint8_t g, uint8_t b)
{
    int offset = 0;
    if(eye != 0)
        offset = EYE_PIXEL_COUNT;
    int row0[] = {11, 12};
    int row1[] = {17, 18, 19};
    int row2[] = {24, 25, 26, 27};
    int row3[] = {32, 33, 34};
    int row4[] = {39, 40};
    
    if(row == 0)
        for(int i=0; i<ARRAYLEN(row0); i++)
            strip.setPixelColor(row0[i] + offset, strip.Color(r, g, b));
    else if(row == 1)
        for(int i=0; i<ARRAYLEN(row1); i++)
            strip.setPixelColor(row1[i] + offset, strip.Color(r, g, b));
    else if(row == 2)
        for(int i=0; i<ARRAYLEN(row2); i++)
            strip.setPixelColor(row2[i] + offset, strip.Color(r, g, b));
    else if(row == 3)
        for(int i=0; i<ARRAYLEN(row3); i++)
            strip.setPixelColor(row3[i] + offset, strip.Color(r, g, b));
    else if(row == 4)
        for(int i=0; i<ARRAYLEN(row4); i++)
            strip.setPixelColor(row4[i] + offset, strip.Color(r, g, b));
}

int drawPupilsRow(int row, uint8_t r, uint8_t g, uint8_t b)
{
    drawPupilRow(0, row, r, g, b);
    drawPupilRow(1, row, r, g, b);
}

int drawPupil(int eye, uint8_t r, uint8_t g, uint8_t b)
{
    for(int i=0; i<5; i++)
        drawPupilRow(eye, i, r, g, b);
}

int drawPupils(uint8_t r, uint8_t g, uint8_t b)
{
    drawPupil(0, r, g, b);
    drawPupil(1, r, g, b);
}

int blink(int del)
{
    closeEyes(del);
    delay(200);
    openEyes(del);
}

int wakeUpFreddy()
{
    int blinkDel = 10;
    
    narrowEyes(100);
    delay(2000);
    blink(blinkDel);
    delay(random(1000));
    blink(blinkDel);
    blink(blinkDel);
    delay(random(2000));
    blink(blinkDel);
    delay(random(2000));
    blink(blinkDel);
    blink(blinkDel);
    delay(random(1000));
    closeEyes(50);
    delay(random(1000));
    blink(blinkDel);
    delay(5000);
}

int glowEyeFreddy(int del)
{
    for(int j=0; j<10; j++)
    {
        for(int i=30; i>0; i--)
        {
            drawPupils(75 / i, 0, 130 / i);
            strip.show();   
            delay(20);
        }
        
        for(int i=1; i<30; i++)
        {
            drawPupils(75 / i, 0, 130 / i);
            strip.show();   
            delay(20);
        }
    }
}

int drawHeartRow(int eye, int row, uint8_t r, uint8_t g, uint8_t b)
{
    int offset = 0;
    if(eye != 0)
        offset = EYE_PIXEL_COUNT;
    int row0[] = {4, 5, 7, 8};
    int row1[] = {9, 10, 11, 12, 13, 14};
    int row2[] = {16, 17, 18, 19, 20};
    int row3[] = {23, 24, 25, 26, 27, 28};
    int row4[] = {31, 32, 33, 34, 35};
    int row5[] = {38, 39, 40, 41};
    int row6[] = {44, 45, 46};
    int row7[] = {49, 50};
    
    if(row == 0)
        for(int i=0; i<ARRAYLEN(row0); i++)
            strip.setPixelColor(row0[i] + offset, strip.Color(r, g, b));
    else if(row == 1)
        for(int i=0; i<ARRAYLEN(row1); i++)
            strip.setPixelColor(row1[i] + offset, strip.Color(r, g, b));
    else if(row == 2)
        for(int i=0; i<ARRAYLEN(row2); i++)
            strip.setPixelColor(row2[i] + offset, strip.Color(r, g, b));
    else if(row == 3)
        for(int i=0; i<ARRAYLEN(row3); i++)
            strip.setPixelColor(row3[i] + offset, strip.Color(r, g, b));
    else if(row == 4)
        for(int i=0; i<ARRAYLEN(row4); i++)
            strip.setPixelColor(row4[i] + offset, strip.Color(r, g, b));
    else if(row == 5)
        for(int i=0; i<ARRAYLEN(row5); i++)
            strip.setPixelColor(row5[i] + offset, strip.Color(r, g, b));
    else if(row == 6)
        for(int i=0; i<ARRAYLEN(row6); i++)
            strip.setPixelColor(row6[i] + offset, strip.Color(r, g, b));
    else if(row == 7)
        for(int i=0; i<ARRAYLEN(row7); i++)
            strip.setPixelColor(row7[i] + offset, strip.Color(r, g, b));
}

int drawHeartEye(uint8_t r, uint8_t g, uint8_t b)
{
    for(int i=0; i<8; i++)
    {
        drawHeartRow(0, i, r, g, b);
        drawHeartRow(1, i, r, g, b);
    }
    strip.show();
}

int heartEyeFreddy(uint8_t r, uint8_t g, uint8_t b)
{
    int del = 5;
    for(int j=0; j<10; j++)
    {
        for(int k=0; k<2; k++)
        {
            for(int i=20; i>0; i--)
            {
                drawHeartEye(75 / i, 0, 130 / i);
                strip.show();   
                delay(del);
            }
            
            for(int i=1; i<20; i++)
            {
                drawHeartEye(75 / i, 0, 130 / i);
                strip.show();   
                delay(del);
            }
        }
        
        delay(1000);
    }
    
    // for(int i=0; i<50; i++)
    // {
    //     drawHeartEye(random(128), random(128), random(128));
    //     strip.show();
    //     delay(60);
    //     drawHeartEye(0, 0, 0);
    //     strip.show();
    //     delay(60);
    // }
    
    drawHeartEye(75, 0, 130);
    strip.show();
}

int sparkleFreddy()
{
    closeEyes(100);
    delay(1000);
    blink(100);
    closeEyes(100);
    for(int j=0; j<100; j++)
    {
        int cnt = 10;
        int pixNums[cnt];
        for(int i=0; i<cnt; i++)
        {
            int pix = random(strip.numPixels());
            pixNums[i] = pix;
            strip.setPixelColor(pix, strip.Color(255,255,255));
        }
        strip.show();
        delay(30);
        for(int i=0; i<cnt; i++)
        {
            int pix = pixNums[i];
            strip.setPixelColor(pix, strip.Color(255,0,0));
        }
        strip.show();
        delay(30);
    }
    blink(100);
    openEyes(100);
    delay(2000);
}

int spiralFreddy(uint8_t r, uint8_t g, uint8_t b, uint8_t r1, uint8_t g1, uint8_t b1)
{
    int offset = EYE_PIXEL_COUNT;
    int del = 50;
        
    int order[] = {18, 25, 33, 26, 11, 24, 17, 32, 39, 40, 34, 27, 19, 12, 5, 10, 16, 31, 38, 44, 45, 46, 41, 35, 20, 13, 7, 6, 1, 0, 4, 9, 23, 37, 43, 48, 49, 50, 51, 47, 42, 28, 14, 8, 3, 2};
    int order0[] = {18, 25, 33, 26};
    int order1[] = {11, 17, 24, 32, 39, 40, 34, 27, 19, 12};
    int order2[] = {5, 10, 16, 31, 38, 44, 45, 46, 41, 35, 20, 13, 7, 6};
    int order3[] = {1, 0, 4, 9, 23, 37, 43, 48, 49, 50, 51, 47, 42, 28, 14, 8, 3, 2};
    for(int i=0; i<ARRAYLEN(order0); i++)
    {
        strip.setPixelColor(order0[i], strip.Color(r, g, b)); 
        strip.setPixelColor(order0[i] + offset, strip.Color(r, g, b));
    }
    strip.show();
    delay(del);
    for(int i=0; i<ARRAYLEN(order1); i++)
    {
        strip.setPixelColor(order1[i], strip.Color(r + 10, g + 10, b + 10));
        strip.setPixelColor(order1[i] + offset, strip.Color(r + 10, g + 10, b + 10)); 
    }
    strip.show();
    delay(del);
    for(int i=0; i<ARRAYLEN(order2); i++)
    {
        strip.setPixelColor(order2[i], strip.Color(r + 20, g + 20, b + 20)); 
        strip.setPixelColor(order2[i] + offset, strip.Color(r + 20, g + 20, b + 20)); 
    }
    strip.show();
    delay(del);
    for(int i=0; i<ARRAYLEN(order3); i++)
    {
        strip.setPixelColor(order3[i], strip.Color(r + 30, g + 30, b + 30)); 
        strip.setPixelColor(order3[i] + offset, strip.Color(r + 30, g + 30, b + 30)); 
    }
    strip.show();
    delay(del);
    

    for(int i=0; i<ARRAYLEN(order3); i++)
    {
        strip.setPixelColor(order3[i], strip.Color(r1, g1, b1)); 
        strip.setPixelColor(order3[i] + offset, strip.Color(r1, g1, b1)); 
    }
    strip.show();
    delay(del);
    for(int i=0; i<ARRAYLEN(order2); i++)
    {
        strip.setPixelColor(order2[i], strip.Color(r1, g1, b1)); 
        strip.setPixelColor(order2[i] + offset, strip.Color(r1, g1, b1)); 
    }
    strip.show();
    delay(del);
    for(int i=0; i<ARRAYLEN(order1); i++)
    {
        strip.setPixelColor(order1[i], strip.Color(r1, g1, b1)); 
        strip.setPixelColor(order1[i] + offset, strip.Color(r1, g1, b1)); 
    }
    strip.show();
    delay(del);
    for(int i=0; i<ARRAYLEN(order0); i++)
    {
        strip.setPixelColor(order0[i], strip.Color(r1, g1, b1)); 
        strip.setPixelColor(order0[i] + offset, strip.Color(r1, g1, b1));
    }
    strip.show();
    delay(del);
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

int rainbow(int d) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & MAX_COLOR));
  }
  strip.show();
  delay(d);
}
return 1;
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

int fadeColor(uint8_t r1, uint8_t g1, uint8_t b1,
  uint8_t r2, uint8_t g2, uint8_t b2,
  int del, int duration)
{

    int16_t redDiff = r2 - r1;
    int16_t greenDiff = g2 - g1;
    int16_t blueDiff = b2 - b1;

    int16_t steps = duration / del;

    int16_t redValue, greenValue, blueValue;

    for (int16_t i = steps; i >= 0; i--) {
        redValue = r1 + (redDiff * i / steps);
        greenValue = g1 + (greenDiff * i / steps);
        blueValue = b1 + (blueDiff * i / steps);

        for (uint16_t i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(redValue, greenValue, blueValue));
        }
        strip.show();
        delay(del);
    }

    return 1;
}
