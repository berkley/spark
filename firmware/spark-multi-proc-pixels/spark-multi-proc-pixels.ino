#include "neopixel/neopixel.h"
#include "Spark-Websockets/Spark-Websockets.h"
#include "application.h"

WebSocketClient client;
// char server[] = "10.0.1.6"; //dino
char server[] = "10.0.1.8"; //syncline
// char server[] = "192.168.1.145"; //albina press wifi
#define WS_PORT 3001
#define PIXEL_PIN D2
#define PIXEL_COUNT 256
#define WIDTH 32
#define HEIGHT 8
#define BRIGHTNESS_FACTOR 1
#define PIXEL_TYPE WS2812B
#define PARAM_ARR_SIZE 265
#define SERIAL_WIRING 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// String* strArr = new String[265];
int paramArr[PARAM_ARR_SIZE];

String getCoreID()
{
  String coreIdentifier = "";
  char id[12];
  memcpy(id, (char *)ID1, 12);
  char hex_digit;
  for (int i = 0; i < 12; ++i)
  {
    hex_digit = 48 + (id[i] >> 4);
    if (57 < hex_digit)
     hex_digit += 39;
     coreIdentifier = coreIdentifier + hex_digit;
    hex_digit = 48 + (id[i] & 0xf);
   if (57 < hex_digit)
     hex_digit += 39;
   coreIdentifier = coreIdentifier + hex_digit;
 }
 return coreIdentifier;
}

void onMessage(WebSocketClient client, char* message) {
    Serial.print("Received: ");
    Serial.println(message);
    
    // String* vals = stringSplit(message, ',');
    int* vals = stringSplit(message, ',');
    
    if(vals[0] == -99) //so lame...but it works
    {
        String s1 = "{\"coreid\":\"";
        String s2 = getCoreID();
        String s3 = "\"}";
        s1.concat(s2);
        s1.concat(s3);
        char* str = new char[s1.length() + 1];
        strcpy(str, s1.c_str());
        Serial.println(s1);
        client.send(str);
        return;
    } 
    else if(vals[0] == -98)
    { //set full screen to one color
        Serial.println("Setting full screen -98");
        for(int i=0; i<PIXEL_COUNT; i++)
        {
            strip.setPixelColor(i, strip.Color(vals[1], vals[2], vals[3]));
        }
        strip.show();
    }
    else if(vals[0] == -97)
    { //write the bitmap to the array starting at column vals[1]
        //key: vals[0]: -97
        //     vals[1]: reset [1|0] //set the screen to all off before writing the bmp
        //     vals[2]: upperLeft
        //     vals[3]: bmp width
        //     vals[4]: bmp height
        //     vals[5]: r1
        //     vals[6]: g1
        //     vals[7]: b1
        //     etc
        Serial.print("Setting Bitmap at ");
        Serial.println(vals[2]);
        if(vals[1] == 1)
        {
            setAllOff();
        }
        setBitmap(vals, vals[3], vals[4], vals[2]);
    }
    else
    {
        strip.setPixelColor(vals[0], strip.Color(vals[1], vals[2], vals[3]));
        strip.show();     
    }
}

void setAllOff()
{
    for(int i=0; i<PIXEL_COUNT; i++)
    {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
}

void setBitmap(int* bmp, int width, int height, int upperLeft)
{
    int index = 5; //cut off the metadata at the beginning of the array
    Serial.println("setting bmp...");
    Serial.print("width: ");
    Serial.println(width);
    Serial.print("height: ");
    Serial.println(height);
    Serial.print("upperLeft: ");
    Serial.println(upperLeft);
    
    for(int y=0; y<height; y++)
    {
        for(int x=upperLeft; x<(width + upperLeft); x++)
        {
            int addr;
            addr = getPixelAddress(y, x);
            Serial.print("x: ");
            Serial.println(x);
            Serial.print("y: ");
            Serial.println(y);
            Serial.print("addr: ");
            Serial.println(addr);
            int r = bmp[index + 0] / BRIGHTNESS_FACTOR;
            int g = bmp[index + 1] / BRIGHTNESS_FACTOR;
            int b = bmp[index + 2] / BRIGHTNESS_FACTOR;
            // Serial.print("r: ");
            // Serial.println(r);
            // Serial.print("g: ");
            // Serial.println(g);
            // Serial.print("b: ");
            // Serial.println(b);
            strip.setPixelColor(addr, strip.Color(r, g, b));
            index += 3;
        }
    }
    
    strip.show();
}

int getPixelAddress(int row, int col)
{
    if(SERIAL_WIRING)
    {
        int addr;
        if(row % 2 == 0)
        {
          addr = (row * WIDTH) + col;
        }
        else
        {
          int offset = WIDTH - ((col * 2) + 1) ;
          addr = ((row * WIDTH) + col) + offset;
        }
        return addr;
    }
    else
    {
        return (row * WIDTH) + col;
    }
}

void onError(WebSocketClient client, char* message) {
    Serial.print("Error: ");
    Serial.println(message);
}

void onOpen(WebSocketClient client) {
    Serial.print("Connection Opened on port ");
    Serial.println(WS_PORT);
}

void onClose(WebSocketClient client, int code, char* message) {
    Serial.println("Connection closed");
    Serial.print("reopening connection on port");
    Serial.println(WS_PORT);
    client.connect(server, WS_PORT);
}

void setup() 
{
    Serial.begin(9600);
    Serial.println("Setup complete");
    strip.begin();
    strip.show();
    client.connect(server, WS_PORT);
    client.onMessage(onMessage);
    client.onError(onError);
    client.onClose(onClose);
    client.onOpen(onOpen);
    for(int i=0; i<PARAM_ARR_SIZE; i++)
    {
        paramArr[i] = 0;
    }
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.show();
}

void loop() {
    client.monitor();
}

int* stringSplit(String s, char delim)
{
    int arrcnt = 0;
    String token = "";
    for(int i=0; i<s.length(); i++)
    {
        char c = s.charAt(i);
        if(c == delim)
        {
            paramArr[arrcnt] = token.toInt();
            arrcnt++;
            token = "";
        }
        else
        {
            token.concat(String(c));
        }
    }
    paramArr[arrcnt] = token.toInt();
    return paramArr;
}
