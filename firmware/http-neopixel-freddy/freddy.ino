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
// #define PIXEL_COUNT 150 //colossus
#define PIXEL_COUNT 104 //FREDDY
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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

TCPServer server = TCPServer(TCP_PORT);
TCPClient client;

#define MAX_COLOR 255

char action[64];
char parameters[64];
char ip[64];
char tcpstate[64];

String loopRun = STOP;
String *loopArgs = new String[20];
String *strArr = new String[20];

void setup()
{
  Serial.begin(9600);
  WiFi.selectAntenna(ANT_EXTERNAL);
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
    else
    {
        String("INVALID_ACTION").toCharArray(action, 64);
        loopRun = STOP;
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

