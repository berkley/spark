#include "application.h"
#include "neopixel/neopixel.h"
#include "led-strip-particles.h"

//the pin your spark is using to control neopixels
#define PIXEL_PIN D2
//the number of pixels you are controlling
#define PIXEL_COUNT 467 //giggle
// #define PIXEL_COUNT 150 //colossus

//the neopixel chip type
#define PIXEL_TYPE WS2812B

//particle params
#define MAX_COLOR 255
#define NUM_PARTICLES 12
#define FPS 30
#define MILLIS_PER_FRAME (1000 / FPS)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
ParticleEmitter emitter = ParticleEmitter(PIXEL_COUNT, MAX_COLOR);
char action[64];
char parameters[64];

void setCoordColor(Coord3D coord, uint32_t color);

//program and action names
#define STOP "stop"
#define SHUTDOWN "shutdown"
#define PRESTOP "prestop"
#define RAINBOW "rainbow"
#define ALTERNATE "alternate"
#define BLOCKS "blocks"
#define FADECOLOR "fadeColor"
#define PARTICLES "particles"
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

String loopRun = STOP;
String *loopArgs = new String[20];
String *strArr = new String[20];

void setup() 
{
//   Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  //register the run command as an API endpoint
  Spark.function("run", run);
  //register the action variable as a GET parameter
  Spark.variable("action", &action, STRING);
  //retister the parameters variable as a GET parameter
  Spark.variable("parameters", &parameters, STRING);
}

void loop() 
{
    if(loopRun.equals(STOP))
    {
        delay(1000);
    }
    else if(loopRun.equals(SHUTDOWN))
    { //stop all programs and set all pixels to off
        loopRun = PRESTOP;
        delay(1000); //give a program time to stop
    }
    else if(loopRun.equals(PRESTOP))
    {
        loopRun = STOP;
        allOff();
    }
    else if(loopRun.equals(RAINBOW))
    {
        rainbow(20);        
    }
    else if(loopRun.equals(ALTERNATE))
    {
        int r1 = stringToInt(loopArgs[0]);
        int g1 = stringToInt(loopArgs[1]);
        int b1 = stringToInt(loopArgs[2]);
        int r2 = stringToInt(loopArgs[3]);
        int g2 = stringToInt(loopArgs[4]);
        int b2 = stringToInt(loopArgs[5]);
        int d = stringToInt(loopArgs[6]);

        staticAlternate(r1, g1, b1, r2, g2, b2);
        delay(d);
        staticAlternate(r2, g2, b2, r1, g1, b1);
        delay(d);
    }
    else if(loopRun.equals(BLOCKS))
    {
        int r1 = stringToInt(loopArgs[0]);
        int g1 = stringToInt(loopArgs[1]);
        int b1 = stringToInt(loopArgs[2]);
        int r2 = stringToInt(loopArgs[3]);
        int g2 = stringToInt(loopArgs[4]);
        int b2 = stringToInt(loopArgs[5]);
        int d = stringToInt(loopArgs[6]);
        int blockSize = stringToInt(loopArgs[7]);
        
        animateBlocks(r2, g2, b2, r1, g1, b1, blockSize, d, true);
        animateBlocks(r2, g2, b2, r1, g1, b1, blockSize, d, false);
    }
    else if(loopRun.equals(FADECOLOR))
    {
        int r1 = stringToInt(loopArgs[0]);
        int g1 = stringToInt(loopArgs[1]);
        int b1 = stringToInt(loopArgs[2]);
        int r2 = stringToInt(loopArgs[3]);
        int g2 = stringToInt(loopArgs[4]);
        int b2 = stringToInt(loopArgs[5]);
        int d = stringToInt(loopArgs[6]);
        int duration = stringToInt(loopArgs[7]);
        
        fadeColor(r1, g1, b1, r2, g2, b2, d, duration);
        delay(d);
        fadeColor(r2, g2, b2, r1, g1, b1, d, duration);
        delay(d);
    }
    else if(loopRun.equals(PARTICLES))
    {
        particles(); 
    }
    else if(loopRun.equals(ENDRUN))
    {
        int r1 = stringToInt(loopArgs[0]);
        int g1 = stringToInt(loopArgs[1]);
        int b1 = stringToInt(loopArgs[2]);
        int r2 = stringToInt(loopArgs[3]);
        int g2 = stringToInt(loopArgs[4]);
        int b2 = stringToInt(loopArgs[5]);
        int d = stringToInt(loopArgs[6]);
        endRun(r1, g1, b1, r2, g2, b2, d);
    }
    else if(loopRun.equals(SNOW))
    {
        snow();
    }
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
    else if(command.equals(PARTICLES))
    {
        int np = stringToInt(args[1]);
        int mv = stringToInt(args[2]);
        bool respawn = stringToBool(args[3]);
        bool threed = stringToBool(args[4]);
        bool flicker = stringToBool(args[5]);
        
        emitter.respawnOnOtherSide = respawn;
        emitter.threed = threed;
        emitter.flicker = flicker;
        emitter.numParticles = np;
        float mvf = mv / 10.0;
        emitter.maxVelocity = mvf / FPS;
        loopRun = PARTICLES;
        return 1;
    }
    else if(command.equals(SETBRIGHTNESS))
    {
        int brightness = stringToInt(args[1]);
        strip.setBrightness(brightness);
        strip.show();
    }
    else 
    { //command not found
        return 0;
    }
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

void particles() {
    unsigned long frameStartMillis = millis();
    emitter.stripPosition = 0.5; //random(100) / 100.0;

    // Draw each particle
    for (int i=0; i < emitter.numParticles; i++) {

        // Update this particle's position
        Particle prt = emitter.updateParticle(i);

        float zScale = (emitter.threed ? (1.0 - prt.coord.z) : 1.0);
        double tailLength = (1.0 + abs(prt.velocity.x * 15) * zScale);
        int16_t startSlot = emitter.numPixels * prt.coord.x;
        int16_t currentSlot = startSlot;
        int16_t oldSlot = currentSlot;

        // Draw the particle and its tail
        // High velocity particles have longer tails
        for (int ii=0; ii < tailLength; ii++) {

            // Taper the tail fade
            float colorScale = ((tailLength - (ii * GOLDEN_RATIO)) / tailLength);

            if (ii == 0 && prt.dimmed) {
            // Flicker the first particle
                colorScale *= (random(50) / 100) + 0.05;
            }      

            if (emitter.threed) {
                colorScale *= zScale;
            }

            if (colorScale < 0.05) {
                colorScale = 0.0;
            }

            // Draw particle
            strip.setPixelColor(currentSlot, 
                                strip.Color(prt.redColor * colorScale, 
                                            prt.greenColor * colorScale, 
                                            prt.blueColor * colorScale));

            oldSlot = currentSlot;
            currentSlot = startSlot + ((ii+1) * (prt.velocity.x > 0 ? -1 : 1));
        }

        //Terminate the tail
        strip.setPixelColor(oldSlot, strip.Color(0, 0, 0));
    }

    uint16_t frameElapsedMillis = millis() - frameStartMillis;
    uint16_t frameDelayMillis = (MILLIS_PER_FRAME - frameElapsedMillis);

    if (frameDelayMillis > 0.0) {
        Serial.println(frameDelayMillis);
        delay(frameDelayMillis);
        strip.show();
    }
}

void setCoordColor(Coord3D coord, uint32_t color) {
    strip.setPixelColor(coord.x * emitter.numPixels, color); 
}

