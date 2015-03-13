#include "Spark-Websockets.h"
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

void onMessage(WebSocketClient client, char* message) {
    Serial.print("Received: ");
    Serial.println(message);
    
    client.send("ok");
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
    client.connect(server, WS_PORT);
    client.onMessage(onMessage);
    client.onError(onError);
    client.onClose(onClose);
    client.onOpen(onOpen);
}

void loop() {
    client.monitor();
}


