#include "Spark-Websockets.h"
#include "application.h"

WebSocketClient client;
// char server[] = "10.0.1.6"; //dino
char server[] = "10.0.1.8"; //syncline
// char server[] = "192.168.1.145"; //albina press wifi

#define WS_PORT 3001

void onMessage(WebSocketClient client, char* message) {
    Serial.print("Received: ");
    Serial.println(message);
    
    // while(true)
    // {
    //     client.send("yay!");
    //     delay(1000);
    // }
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
    Serial.print("Connecting to server ");
    Serial.print(server);
    Serial.print(" on port ");
    Serial.println(WS_PORT);
    client.connect(server, WS_PORT);
    client.onMessage(onMessage);
    client.onError(onError);
    client.onClose(onClose);
    client.onOpen(onOpen);
}

void loop() {
    client.monitor();
}


