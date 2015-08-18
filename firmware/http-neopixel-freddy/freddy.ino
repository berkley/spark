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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

TCPServer server = TCPServer(TCP_PORT);
TCPClient client;

#define MAX_COLOR 255

char action[64];
char parameters[64];
char ip[64];
char tcpstate[64];

//program and action names
#define STOP "stop"
#define SHUTDOWN "shutdown"
#define PRESTOP "prestop"
#define RAINBOW "rainbow"
#define ALTERNATE "alternate"
#define BLOCKS "blocks"
#define FADECOLOR "fadeColor"
#define ALLOFF "allOff"
#define SETALL "setAll"
#define LOOPALTERNATE "loopAlternate"
#define LOOPBLOCKS "loopBlocks"
#define LATCHPIXEL "latchPixel"
#define SETPIXEL "setPixel"
#define LATCH "latch"
#define ENDRUN "endrun"
#define SNOW "snow"
#define WEBSOCKET "websocket" //<- finish this
#define SETBRIGHTNESS "setBrightness"
#define USA "usa"
#define FREDDYMAD "freddymad"

String loopRun = FREDDYMAD;
String *loopArgs = new String[20];
String *strArr = new String[20];

void setup()
{
//   Serial.begin(9600);
  server.begin();
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  //register the run command as an API endpoint
  Spark.function("run", run);
  //register the action variable as a GET parameter
  Spark.variable("action", &action, STRING);
  Spark.variable("tcpstate", &tcpstate, STRING);
  //retister the parameters variable as a GET parameter
  Spark.variable("parameters", &parameters, STRING);
  Spark.variable("ip", &ip, STRING);
  (String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." +  String(WiFi.localIP()[2]) + "." +  String(WiFi.localIP()[3])).toCharArray(ip, 64);
}

void loop()
{
    if (client.connected()) {
        String("CONNECTED").toCharArray(tcpstate, 64);
        String dStr = "data:";
        char data;
        while (client.available()) 
        {
            data = client.read();
            dStr += String(data);
        }
        client.println("OK: " + dStr);
        client.flush();
        dStr.toCharArray(action, 64);
    } 
    else 
    {
        // if no client is yet connected, check for a new connection
        String("DISCONNECTED").toCharArray(tcpstate, 64);
        client = server.available();
    }
      
    // if(loopRun.equals(STOP))
    // {
    //     delay(1000);
    // }
    // else if(loopRun.equals(SHUTDOWN))
    // { //stop all programs and set all pixels to off
    //     loopRun = PRESTOP;
    //     delay(1000); //give a program time to stop
    // }
    // else if(loopRun.equals(PRESTOP))
    // {
    //     loopRun = STOP;
    //     allOff();
    // }
    // else if(loopRun.equals(RAINBOW))
    // {
    //     rainbow(20);
    // }
    // else if(loopRun.equals(ALTERNATE))
    // {
    //     int r1 = stringToInt(loopArgs[0]);
    //     int g1 = stringToInt(loopArgs[1]);
    //     int b1 = stringToInt(loopArgs[2]);
    //     int r2 = stringToInt(loopArgs[3]);
    //     int g2 = stringToInt(loopArgs[4]);
    //     int b2 = stringToInt(loopArgs[5]);
    //     int d = stringToInt(loopArgs[6]);

    //     staticAlternate(r1, g1, b1, r2, g2, b2);
    //     delay(d);
    //     staticAlternate(r2, g2, b2, r1, g1, b1);
    //     delay(d);
    // }
    // else if(loopRun.equals(BLOCKS))
    // {
    //     int r1 = stringToInt(loopArgs[0]);
    //     int g1 = stringToInt(loopArgs[1]);
    //     int b1 = stringToInt(loopArgs[2]);
    //     int r2 = stringToInt(loopArgs[3]);
    //     int g2 = stringToInt(loopArgs[4]);
    //     int b2 = stringToInt(loopArgs[5]);
    //     int d = stringToInt(loopArgs[6]);
    //     int blockSize = stringToInt(loopArgs[7]);

    //     animateBlocks(r2, g2, b2, r1, g1, b1, blockSize, d, true);
    //     animateBlocks(r2, g2, b2, r1, g1, b1, blockSize, d, false);
    // }
    // else if(loopRun.equals(FADECOLOR))
    // {
    //     int r1 = stringToInt(loopArgs[0]);
    //     int g1 = stringToInt(loopArgs[1]);
    //     int b1 = stringToInt(loopArgs[2]);
    //     int r2 = stringToInt(loopArgs[3]);
    //     int g2 = stringToInt(loopArgs[4]);
    //     int b2 = stringToInt(loopArgs[5]);
    //     int d = stringToInt(loopArgs[6]);
    //     int duration = stringToInt(loopArgs[7]);

    //     fadeColor(r1, g1, b1, r2, g2, b2, d, duration);
    //     delay(d);
    //     fadeColor(r2, g2, b2, r1, g1, b1, d, duration);
    //     delay(d);
    // }
    // else if(loopRun.equals(ENDRUN))
    // {
    //     int r1 = stringToInt(loopArgs[0]);
    //     int g1 = stringToInt(loopArgs[1]);
    //     int b1 = stringToInt(loopArgs[2]);
    //     int r2 = stringToInt(loopArgs[3]);
    //     int g2 = stringToInt(loopArgs[4]);
    //     int b2 = stringToInt(loopArgs[5]);
    //     int d = stringToInt(loopArgs[6]);
    //     endRun(r1, g1, b1, r2, g2, b2, d);
    // }
    // else if(loopRun.equals(SNOW))
    // {
    //     snow();
    // }
    // else if(loopRun.equals(USA))
    // {
    //     runUSA();
    // }
    // else if(loopRun.equals(FREDDYMAD))
    // {
        // runFreddyMad();
    // }
}

int allOff()
{
    setAll(0,0,0);
    return 1;
}

/*
    This function handles the API requests to /run.  The params are a
    comma separated list.
    params format is <command>,<param0>,<param1>,...,<paramN>
    where <command> is the action to be run and <paramN> are the parameters that an individual
    action needs.

    Actions can be divided into two categories: one where the loop() function is used to animate pixel
    values and one where pixel values are set a single time.
    Looped programs can be run using the loopRun variable.  If loopRun is set to STOP the loop will
    run with no program selected.
*/
int run(String params)
{
    String* args = stringSplit(params, ',');
    String command = args[0];
    strcpy(parameters, params.c_str());
    strcpy(action, command.c_str());

    if(command.equals(ALLOFF))
    {
        loopRun = SHUTDOWN;
    }
    else if(command.equals(SETALL))
    {
        int r = stringToInt(args[1]);
        int g = stringToInt(args[2]);
        int b = stringToInt(args[3]);
        return setAll(r, g, b);
    }
    else if(command.equals(ALTERNATE))
    {
        int r1 = stringToInt(args[1]);
        int g1 = stringToInt(args[2]);
        int b1 = stringToInt(args[3]);
        int r2 = stringToInt(args[4]);
        int g2 = stringToInt(args[5]);
        int b2 = stringToInt(args[6]);
        return staticAlternate(r1, g1, b1, r2, g2, b2);
    }
    else if(command.equals(LOOPALTERNATE))
    {
        loopRun = ALTERNATE;
        loopArgs[0] = args[1]; //r1
        loopArgs[1] = args[2]; //g1
        loopArgs[2] = args[3]; //b1
        loopArgs[3] = args[4]; //r2
        loopArgs[4] = args[5]; //g2
        loopArgs[5] = args[6]; //b2
        loopArgs[6] = args[7]; //delay
        return 1;
    }
    else if(command.equals(LOOPBLOCKS))
    {
        loopRun = BLOCKS;
        loopArgs[0] = args[1]; //r1
        loopArgs[1] = args[2]; //g1
        loopArgs[2] = args[3]; //b1
        loopArgs[3] = args[4]; //r2
        loopArgs[4] = args[5]; //g2
        loopArgs[5] = args[6]; //b2
        loopArgs[6] = args[7]; //delay
        loopArgs[7] = args[8]; //block size
        return 1;
    }
    else if(command.equals(BLOCKS))
    {
        int r1 = stringToInt(args[1]);
        int g1 = stringToInt(args[2]);
        int b1 = stringToInt(args[3]);
        int r2 = stringToInt(args[4]);
        int g2 = stringToInt(args[5]);
        int b2 = stringToInt(args[6]);
        int blockSize = stringToInt(args[7]);
        buildBlocks(r1, g1, b1, r2, g2, b2, blockSize);
        return 1;
    }
    else if(command.equals(FADECOLOR))
    {
        //possible commands: stop, rainbow, alternate
        loopRun = FADECOLOR;
        loopArgs[0] = args[1]; //r1
        loopArgs[1] = args[2]; //g1
        loopArgs[2] = args[3]; //b1
        loopArgs[3] = args[4]; //r2
        loopArgs[4] = args[5]; //g2
        loopArgs[5] = args[6]; //b2
        loopArgs[6] = args[7]; //delay
        loopArgs[7] = args[8]; //duration
        return 1;
    }
    else if(command.equals(RAINBOW))
    {
        loopRun = RAINBOW;
        return 1;
    }
    else if(command.equals(LATCHPIXEL))
    {
        int pixel = stringToInt(args[0]);
        int r1 = stringToInt(args[1]);
        int g1 = stringToInt(args[2]);
        int b1 = stringToInt(args[3]);
        strip.setPixelColor(pixel, strip.Color(r1, g1, b1));
        strip.show();
        return 1;
    }
    else if(command.equals(SETPIXEL))
    {
        int pixel = stringToInt(args[0]);
        int r1 = stringToInt(args[1]);
        int g1 = stringToInt(args[2]);
        int b1 = stringToInt(args[3]);
        strip.setPixelColor(pixel, strip.Color(r1, g1, b1));
        return 1;
    }
    else if(command.equals(LATCH))
    {
        strip.show();
        return 1;
    }
    else if(command.equals(STOP))
    {
        loopRun = STOP;
        return 1;
    }
    else if(command.equals(SHUTDOWN))
    {
        loopRun = SHUTDOWN;
        return 1;
    }
    else if(command.equals(ENDRUN))
    {
        loopRun = ENDRUN;
        loopArgs[0] = args[1]; //r1
        loopArgs[1] = args[2]; //g1
        loopArgs[2] = args[3]; //b1
        loopArgs[3] = args[4]; //r2
        loopArgs[4] = args[5]; //g2
        loopArgs[5] = args[6]; //b2
        loopArgs[6] = args[7]; //delay
        return 1;
    }
    else if(command.equals(SNOW))
    {
        loopRun = SNOW;
        return 1;
    }
    else if(command.equals(SETBRIGHTNESS))
    {
        int brightness = stringToInt(args[1]);
        strip.setBrightness(brightness);
        strip.show();
        return 1;
    }
    else if(command.equals(USA))
    {
        loopRun = USA;
        return 1;
    }
    else if(command.equals(FREDDYMAD))
    {
        loopRun = FREDDYMAD;
        return 1;
    }
    else
    { //command not found
        return 0;
    }
}

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
    for(int j=0; j<25; j++)
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
    
    delay(5000);
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

int runFreddyMad()
{
    // drawHeartEye(35, 0, 60);
    // strip.show();
    // delay(5000);
    
    // for(int i=0; i<50; i++)
    //     spiralFreddy(random(128), random(128), random(255), random(50), random(50), random(50));
        
    // blink(100);
    // delay(1000);
    
    // drawHeartEye(75, 0, 130);
    // strip.show();
    // delay(5000);
    
    // sparkleFreddy();
    // delay(1000);
    
    heartEyeFreddy(75, 0, 130);
    delay(1000);
    
    // rainbow(20);
    // delay(1000);
    
    // wakeUpFreddy();
    // delay(1000);
    
    // glowEyeFreddy(20);
    // delay(1000);
    
    return 1;
}

int runUSA()
{
    int blockSize = 5;

    // for(int j=0; j<blockSize; j++)
    int j = 0;
    {
        int count = -1;
        for(int i=0; i<strip.numPixels(); i++) {
            if(i % blockSize == 0)
            {
                count++;
                if(count > 2)
                    count = 0;
            }


            int pix = 0;
            if( (i + j) >= strip.numPixels())
            {
                pix = (i + j) - strip.numPixels();
            }
            else
            {
                pix = i + j;
            }

            if(count == 0)
            {
                strip.setPixelColor(pix, strip.Color(255, 0, 0));
            }
            else if(count == 1)
            {
                strip.setPixelColor(pix, strip.Color(255, 255, 255));
            }
            else if(count == 2)
            {
                strip.setPixelColor(pix, strip.Color(0, 0, 255));
            }

        }
        strip.show();
        delay(500);
    }
    return 1;
}

int snow()
{
    for(int i=0; i<strip.numPixels() / 10; i++)
    { //pick the random pixels
        int pix = random(strip.numPixels());
        strip.setPixelColor(pix, strip.Color(0, 0, 0));
        strip.show();
        delay(random(50));
    }

    for(int i=0; i<strip.numPixels() / 10; i++)
    { //pick the random pixels
        int pix = random(strip.numPixels());
        int brightness = random(255);
        strip.setPixelColor(pix, strip.Color(brightness, brightness, brightness * .20)); //poor mans white balance
        strip.show();
        delay(random(50));
    }
}

int endRun(uint8_t r1, uint8_t g1, uint8_t b1,
    uint8_t r2, uint8_t g2, uint8_t b2,
    uint8_t d)
{
    for(int i=0; i<strip.numPixels(); i++)
    {
        strip.setPixelColor(i, strip.Color(r1, g1, b1));
        strip.setPixelColor(strip.numPixels() - i, strip.Color(r2, g2, b2));
        strip.show();
        delay(d);
    }
}

int animateBlocks(uint8_t r1, uint8_t g1, uint8_t b1,
    uint8_t r2, uint8_t g2, uint8_t b2, uint8_t blockSize, int d, bool inBlock)
{
    for(int j=0; j<blockSize; j++)
    {
        for(int i=0; i<strip.numPixels(); i++) {
            if(i % blockSize == 0)
            {
                inBlock = !inBlock;
            }

            int pix = 0;
            if( (i + j) >= strip.numPixels())
            {
                pix = (i + j) - strip.numPixels();
            }
            else
            {
                pix = i + j;
            }

            if(inBlock)
            {
                strip.setPixelColor(pix, strip.Color(r1, g1, b1));
            }
            else
            {
                strip.setPixelColor(pix, strip.Color(r2, g2, b2));
            }
        }
        strip.show();
        delay(d);
    }

    return 1;
}

int buildBlocks(uint8_t r1, uint8_t g1, uint8_t b1,
    uint8_t r2, uint8_t g2, uint8_t b2,
    uint8_t blockSize)
{
    bool inBlock = true;

    for(int i=0; i<strip.numPixels(); i++) {
        if(i % blockSize == 0)
        {
            inBlock = !inBlock;
        }

        if(inBlock)
        {
            strip.setPixelColor(i, strip.Color(r1, g1, b1));
        }
        else
        {
            strip.setPixelColor(i, strip.Color(r2, g2, b2));
        }
    }
    strip.show();
    return 1;
}

int staticAlternate(uint8_t r1, uint8_t g1, uint8_t b1,
    uint8_t r2, uint8_t g2, uint8_t b2)
{
    for(int i=0; i<strip.numPixels(); i++) {
        if(i % 2 == 0)
        {
            strip.setPixelColor(i, strip.Color(r1, g1, b1));
        }
        else
        {
            strip.setPixelColor(i, strip.Color(r2, g2, b2));
        }
    }
    strip.show();
    return 1;
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

