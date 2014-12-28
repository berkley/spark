#include "neopixel/neopixel.h"
#include "Spark-Websockets/Spark-Websockets.h"
#include "application.h"

WebSocketClient client;
// char server[] = "10.0.1.6"; //dino
char server[] = "10.0.1.22"; //syncline
// char server[] = "192.168.1.145"; //albina press wifi
#define WS_PORT 3001
#define PIXEL_PIN D2
#define PIXEL_COUNT 5
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

String* strArr = new String[20];

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
    
    String* vals = stringSplit(message, ',');
    if(vals[0].toInt() == -99) //so lame...but it works
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
    
    Serial.print("Setting pixel ");
    Serial.println(vals[0]);
    Serial.print("r: ");
    Serial.println(vals[1]);
    Serial.print("g: ");
    Serial.println(vals[2]);
    Serial.print("b: ");
    Serial.println(vals[3]);
    strip.setPixelColor(vals[0].toInt(), strip.Color(vals[1].toInt(), vals[2].toInt(), vals[3].toInt()));
    strip.show();    
}

void onError(WebSocketClient client, char* message) {
    Serial.print("Error: ");
    Serial.println(message);
}

void onOpen(WebSocketClient client) {
    Serial.println("Connection Opened");
}

void onClose(WebSocketClient client, int code, char* message) {
    Serial.println("Connection closed");
    Serial.print("reopening connection on port");
    Serial.println(WS_PORT)
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
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.show();
}

void loop() {
    client.monitor();
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
