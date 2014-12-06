#include "neopixel/neopixel.h"
#include "Spark-Websockets/Spark-Websockets.h"
#include "application.h"

WebSocketClient client;
char server[] = "10.0.1.74";

#define PIXEL_PIN D2
#define PIXEL_COUNT 5
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

String* strArr = new String[20];

void onMessage(WebSocketClient client, char* message) {
    Serial.print("Received: ");
    Serial.println(message);
    
    String* vals = stringSplit(message, ',');
    // if(sizeof(vals) < 4)
    // { //invalid data
    //     return;
    // }
    Serial.print("Setting pixel ");
    Serial.println(vals[0]);
    Serial.print("r: ");
    Serial.println(vals[1]);
    Serial.print("g: ");
    Serial.println(vals[2]);
    Serial.print("b: ");
    Serial.println(vals[3]);
    strip.setPixelColor(stringToInt(vals[0]), strip.Color(stringToInt(vals[1]), stringToInt(vals[2]), stringToInt(vals[3])));
    // strip.setPixelColor(1, strip.Color(random(255), random(255), random(255)));
    strip.show();    
    // free(vals);
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
    Serial.println("reopening connection on port 3001");
    client.connect(server, 3001);
}

void setup() 
{
    Serial.begin(9600);
    Serial.println("Setup complete");
    strip.begin();
    strip.show();
    client.connect(server, 3001);
    client.onMessage(onMessage);
    client.onError(onError);
    client.onClose(onClose);
    client.onOpen(onOpen);
    client.send("Hello World!");
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.show();
}

void loop() {
    client.monitor();
    // strip.setPixelColor(2, strip.Color(0, random(255), 0));
    // strip.show();
    // delay(1000);
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

/*
class WebSocketClient {
public:
  typedef void (*OnMessage)(WebSocketClient client, char* message);
  typedef void (*OnOpen)(WebSocketClient client);
  typedef void (*OnClose)(WebSocketClient client, int code, char* message);
  typedef void (*OnError)(WebSocketClient client, char* message);
  void connect(const char hostname[], int port = 80, const char protocol[] = NULL, const char path[] = "/");
  bool connected();
  void disconnect();
  void monitor();
  void onOpen(OnOpen function);
  void onClose(OnClose function);
  void onMessage(OnMessage function);
  void onError(OnError function);
  bool send(char* message);
private:
*/