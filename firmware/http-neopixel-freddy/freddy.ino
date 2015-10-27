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
#include "neopixel/neopixel.h"

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

String loopRun = KEITHEYES;
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
  
  Serial.println("Serial begin");
  
  pinMode(D5, OUTPUT);
  pinMode(D4, OUTPUT);
  
  //register the action variable as a GET parameter
  Spark.variable("action", &action, STRING);
  Spark.variable("tcpstate", &tcpstate, STRING);
  Spark.variable("parameters", &parameters, STRING);
  Spark.variable("ip", &ip, STRING);
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

