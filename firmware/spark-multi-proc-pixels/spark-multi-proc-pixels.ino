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
#define NUM_BMPS 2
#define SERIAL_WIRING 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// String* strArr = new String[265];
int paramArr[PARAM_ARR_SIZE];

int bitmaps[NUM_BMPS][PARAM_ARR_SIZE];

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
    { //add a bmp to memory
        //key: vals[0]: -97 //funcId
        //     vals[1]: bmp width
        //     vals[2]: bmp height
        //     vals[3]: bmp index //the index to assign to this bmp
        //     vals[3]: r1
        //     vals[4]: g1
        //     vals[5]: b1
        //     etc
        int index = addBitmap(vals);
        if(index == -1)
        {
            client.send("{\"Error\":\"Invalid bmp index\"}");
        }
        Serial.print("Bitmap added successfully at index ");
        Serial.println(index);
        client.send("{\"status\":\"ok\"}");
    }
    else if(vals[0] == -96)
    { //display a bmp at a given location in the array
        //key: vals[0]: -96
        //     vals[1]: column to start display
        //     vals[2]: reset [0 | 1] //reset the display to 0,0,0 before writing the bmp if 1
        //     vals[3]: the bmp index number to display (returned from -97)
        showBitmap(vals);
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

//add a bmp to memory and return the index number
//returns -1 if bmpCount is at NUM_BMPS
int addBitmap(int* bmp)
{
    int index = bmp[3];
    if(index >= NUM_BMPS)
        return -1; //overflow!

    for(int i=0; i<PARAM_ARR_SIZE; i++)
    {
        bitmaps[index][i] = bmp[i];
    }
    return index;
}


void showBitmap(int* vals)
{
    //key: vals[0]: -96
    //     vals[1]: column to start display
    //     vals[2]: reset [0 | 1] //reset the display to 0,0,0 before writing the bmp if 1
    //     vals[3]: the bmp index number to display (returned from -97)
    int upperLeft = vals[1];
    int reset = vals[2];
    int index = vals[3];

    Serial.print("Setting Bitmap at ");
    Serial.println(upperLeft);
    if(reset == 1)
    {
        setAllOff();
    }

    //get the bmp from memory and display it at upperLeft
    //key: vals[0]: -97 //funcId
    //     vals[1]: bmp width
    //     vals[2]: bmp height
    //     vals[3]: bmp index //the index to assign to this bmp
    //     vals[3]: r1
    //     vals[4]: g1
    //     vals[5]: b1
    //     etc
    int* bmp = bitmaps[index];
    int width = bmp[1];
    int height = bmp[2];

    int offset = 4; //cut off the metadata at the beginning of the array
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
            int r = bmp[offset + 0] / BRIGHTNESS_FACTOR;
            int g = bmp[offset + 1] / BRIGHTNESS_FACTOR;
            int b = bmp[offset + 2] / BRIGHTNESS_FACTOR;
            // Serial.print("x: ");
            // Serial.println(x);
            // Serial.print("y: ");
            // Serial.println(y);
            // Serial.print("addr: ");
            // Serial.println(addr);
            // Serial.print("r: ");
            // Serial.println(r);
            // Serial.print("g: ");
            // Serial.println(g);
            // Serial.print("b: ");
            // Serial.println(b);
            strip.setPixelColor(addr, strip.Color(r, g, b));
            offset += 3;
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