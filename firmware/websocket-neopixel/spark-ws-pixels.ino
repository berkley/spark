#include "neopixel.h"
#include "Spark-Websockets.h"
#include "application.h"

SYSTEM_MODE(MANUAL);

WebSocketClient client;
const byte server[] = {10, 0, 1, 6}; //dino
// const byte server[] = {10, 0, 2, 1}; //raspi    
// const byte server[] = {10, 0, 1, 8}; //syncline  
// const byte server[] = {192, 168, 1, 10}; //xfinity

#define WS_PORT 3001
#define PIXEL_PIN D2
#define PIXEL_COUNT 256
#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 8
#define PIXEL_TYPE WS2812B
#define PARAM_ARR_SIZE 265
#define NUM_BMPS 10
#define SERIAL_WIRING 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// String* strArr = new String[265];
static uint8_t paramArr[PARAM_ARR_SIZE];

static uint8_t bitmaps[NUM_BMPS][PARAM_ARR_SIZE];
static String coreId = "";
static String token = "";

String getCoreID()
{
  if(coreId.length() > 0)
  {
    return coreId;
  }
  
  char id[12];
  memcpy(id, (char *)ID1, 12);
  char hex_digit;
  for (int i = 0; i < 12; ++i)
  {
    hex_digit = 48 + (id[i] >> 4);
    if (57 < hex_digit)
     hex_digit += 39;
     coreId = coreId + hex_digit;
    hex_digit = 48 + (id[i] & 0xf);
   if (57 < hex_digit)
     hex_digit += 39;
   coreId = coreId + hex_digit;
 }

 return coreId;
}

static String coreIdJSON = "";
static String messageJSON = "";
static String endPart = "\"}";

char* getCoreIDJSONWithMessageAndTransactionId(String message, int transactionId)
{
    if(coreIdJSON.length() <= 0)
    {
        coreIdJSON = "{\"coreid\":\"";
        coreIdJSON.concat(getCoreID());
        coreIdJSON.concat("\", \"transactionId\":");
    }
    String firstPart = "";
    firstPart.concat(coreIdJSON);
    firstPart.concat(String(transactionId));
    if(messageJSON.length() <= 0)
    {
        messageJSON = ", \"message\":\"";
    }
    firstPart.concat(messageJSON);
    firstPart.concat(message);
    firstPart.concat(endPart);
    
    
    char* str = new char[firstPart.length() + 1];
    strcpy(str, firstPart.c_str());
    
    return str;
}

void onMessage(WebSocketClient client, char* message) {
    Serial.print("Received: ");
    Serial.println(message);
    
    stringSplit(message, ',');
    
    Serial.print("Looking for function # ");
    Serial.println(paramArr[0]);
    
    if(paramArr[0] == 99)
    {
        char *msg = getCoreIDJSONWithMessageAndTransactionId("ident", 0);
        Serial.print("99 returning ");
        Serial.println(msg);
        client.send(msg);
        free(msg);
    } 
    else if(paramArr[0] == 98)
    { //set full screen to one color
        Serial.println("Setting full screen 98");
        for(int i=0; i<PIXEL_COUNT; i++)
        {
            strip.setPixelColor(i, strip.Color(paramArr[1], paramArr[2], paramArr[3]));
        }
        strip.show();
    }
    else if(paramArr[0] == 97)
    { //add a bmp to memory
        //key: vals[0]: 97 //funcId
        //     vals[1]: bmp width
        //     vals[2]: bmp height
        //     vals[3]: bmp index //the index to assign to this bmp
        //     vals[4]: r1
        //     vals[5]: g1
        //     vals[6]: b1
        //     etc
        uint8_t index = addBitmap();
        Serial.print("set bmp at index ");
        Serial.println(index);
    }
    else if(paramArr[0] == 96)
    { //display a bmp at a given location in the array
        //key: vals[0]: 96
        //     vals[1]: column to start display
        //     vals[2]: reset [0 | 1] //reset the display to 0,0,0 before writing the bmp if 1
        //     vals[3]: the bmp index number to display (returned from -97)
        showBitmap();
    }
    else if(paramArr[0] == 95)
    { //turn off all pixels
        setAllOff();
        strip.show();
    }
    else if(paramArr[0] == 94)
    { //set brightness
        strip.setBrightness(paramArr[1]);
        strip.show();
    }
    else
    {
        Serial.println("Function not found");
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
int addBitmap()
{
    int index = paramArr[3];
    Serial.print("Adding bitmap at index ");
    Serial.println(index);
    if(index >= NUM_BMPS)
    {
        Serial.print("Index is >= ");
        Serial.println(NUM_BMPS);
        return -1; //overflow!
    }

    for(int i=0; i<PARAM_ARR_SIZE; i++)
    {
        bitmaps[index][i] = paramArr[i];
    }
    return index;
}


void showBitmap()
{
    //key: vals[0]: 96
    //     vals[1]: column to start display
    //     vals[2]: reset [0 | 1] //reset the display to 0,0,0 before writing the bmp if 1
    //     vals[3]: wraparound [0 | 1] //wrap the bitmap around from 31 to 0. 
    //              If set to false, the bitmap will disappear as it leaves the screen
    //     vals[4]: the bmp index number to display (returned from -97)
    //     vals[5]: negative index [0 | 1] //set to 1 if upperLeft should be treated as a negative number
    int upperLeft = paramArr[1];
    int reset = paramArr[2];
    int wraparound = paramArr[3];
    int index = paramArr[4];
    int negative = paramArr[5];

    if(negative) //to save memory, the bmps are stored as uint8_t, so we need a negative flag for certain cases
        upperLeft *= -1;

    if(reset == 1)
    {
        setAllOff();
    }

    //get the bmp from memory and display it at column
    //key: vals[0]: 97 //funcId
    //     vals[1]: transactionId
    //     vals[2]: bmp width
    //     vals[3]: bmp height
    //     vals[4]: bmp index //the index to assign to this bmp
    //     vals[5]: r1
    //     vals[6]: g1
    //     vals[7]: b1
    //     etc
    uint8_t* bmp = bitmaps[index];
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
    Serial.print("wraparound: ");
    Serial.println(wraparound);

    for(int y=0; y<height; y++)
    {
        for(int x=upperLeft; x<(width + upperLeft); x++)
        {
            int addr = getPixelAddress(y, x, wraparound);
            // Serial.print("Addr: ");
            // Serial.println(addr);
            if(addr == -1)
            {
                Serial.println("addr == -1");
                continue;
            }
            int r = bmp[offset + 0];
            int g = bmp[offset + 1];
            int b = bmp[offset + 2];
            strip.setPixelColor(addr, strip.Color(r, g, b));
            offset += 3;
        }
    }
    
    strip.show();
}

int getPixelAddress(int row, int col, int wraparound)
{
    if(wraparound)
    {
        if(col >= SCREEN_WIDTH)
        {
            col = col - SCREEN_WIDTH;
            // Serial.println("col is greater than screen width and wraparound is true");
        }
        else if(col <= 0)
        {
            // Serial.println("col is less than screen width and wraparound is true");
            col = SCREEN_WIDTH - col;
        }
    }
    else
    {
        if(col >= SCREEN_WIDTH || col <= 0)
        {
            // Serial.println("col is greater than screen width or < 0 and wraparound is false");
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

    for(int i=0; i<PARAM_ARR_SIZE; i++)
    {
        paramArr[i] = 0;
    }
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.show();

    WiFi.on();
    WiFi.connect();
    while(!WiFi.ready())
    {
        Serial.print("Wifi connecting...");
        delay(1000);
    }

    strip.setPixelColor(0, strip.Color(0, 0, 255));
    strip.show();

    client.connect(server, WS_PORT);
    client.onMessage(onMessage);
    client.onError(onError);
    client.onClose(onClose);
    client.onOpen(onOpen);    

    strip.setPixelColor(0, strip.Color(0, 255, 0));
    strip.show();
}

void loop() {
    // Serial.print("loop");
    client.monitor();
}

void stringSplit(String s, char delim)
{
    int arrcnt = 0;
    token = "";
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
    paramArr[arrcnt] = (uint8_t)token.toInt();
}
