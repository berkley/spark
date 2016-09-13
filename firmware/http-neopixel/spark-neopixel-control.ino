/*

LED Map for the Yuba
--------------------

front tube and bottom bracket ground effects - PIXEL_PIN_0
bottom bracket - pixel 0
front tube near spring - pixel 16

Bike - lower rails - PIXEL_PIN_1
back left = 0
center left = 28
center right = 29
back right (last pixel) = 58
Tail - Left: 0-8  Right: 49-58
Sides - Left: 9-28 (19 pixels)  Right: 29-48 (19 pixels)

Headlight - PIXEL_PIN_2
neopixel ring - pixels 0-11

*/

#include "application.h"
#include "neopixel.h"
#include "led-strip-particles.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

#define EEPROM_BRIGHTNESS 0
#define EEPROM_ADDR_PINS 100
#define EEPROM_ADDR_STRIP_0 500
#define EEPROM_ADDR_STRIP_1 1000
#define EEPROM_ADDR_STRIP_2 500

//pins that control on/off functions
#define PIN_0 A0
#define PIN_1 A1
#define PIN_2 A2
#define PIN_3 A3
#define PIN_4 A4
#define PIN_5 A5
#define NUM_PINS 6

//pins for neopixel strips = D0, D1, D2
#define PIXEL_PIN_0 D0
#define PIXEL_PIN_1 D1
#define PIXEL_PIN_2 D2
#define PIXEL_COUNT_0 17
#define PIXEL_COUNT_1 59
#define PIXEL_COUNT_2 12
#define PIXEL_TYPE WS2812B

#define BUTTON_MODE_PIN D6
#define BUTTON_WIFI_PIN D5

#define LED_PIN D7

//particle params
#define MAX_COLOR 255
#define NUM_PARTICLES 12
#define FPS 30
#define MILLIS_PER_FRAME (1000 / FPS)

#define NUM_PARAMS 64
#define NUM_ARGS 20

#define BIKE_LEFT_TAIL_START 0
#define BIKE_LEFT_TAIL_END 5
#define BIKE_RIGHT_TAIL_START 53
#define BIKE_RIGHT_TAIL_END 58
#define BIKE_LEFT_SIDE_START 6
#define BIKE_LEFT_SIDE_END 28
#define BIKE_RIGHT_SIDE_START 29
#define BIKE_RIGHT_SIDE_END 52
#define BIKE_MIDDLE_START 0
#define BIKE_MIDDLE_END 16
#define BIKE_HOOD_START 0
#define BIKE_HOOD_END 12

struct LEDObject
{
    char params[NUM_PARAMS];
};

struct PINObject
{
    int pins[NUM_PINS];
};

uint8_t brightness = 255;
int _brightness = 255;

Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(PIXEL_COUNT_0, PIXEL_PIN_0, PIXEL_TYPE);
LEDObject stripObj0 = {{'I', 'N', 'I', 'T'}};
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(PIXEL_COUNT_1, PIXEL_PIN_1, PIXEL_TYPE);
LEDObject stripObj1 = {{'I', 'N', 'I', 'T'}};
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(PIXEL_COUNT_2, PIXEL_PIN_2, PIXEL_TYPE);
LEDObject stripObj2 = {{'I', 'N', 'I', 'T'}};
//TODO: enable strip0 and strip1.  Right now only strip2 works.

PINObject pinObj = {{0,0,0,0,0,0}};

ParticleEmitter emitter = ParticleEmitter(PIXEL_COUNT_1, MAX_COLOR);

char action[64];
char parameters[NUM_PARAMS];

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
#define ENDRUN "endrun"
#define SNOW "snow"
#define SETBRIGHTNESS "setBrightness"
#define USA "usa"
#define LIGHTNING "lightning"
#define ON "on" 
#define OFF "off"
#define BIKE1 "BIKE1"
#define BIKE2 "BIKE2"

String loopRun = BIKE2;
String *args = new String[NUM_ARGS];
String *loopArgs = new String[NUM_ARGS];
String *strArr = new String[NUM_ARGS];

void setup() 
{
    pinMode(BUTTON_WIFI_PIN, INPUT_PULLUP);
    pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);
    attachInterrupt(BUTTON_WIFI_PIN, connect, CHANGE, 1);
    attachInterrupt(BUTTON_MODE_PIN, mode, CHANGE, 1);

    //retrieve persisted state
    EEPROM.get(EEPROM_ADDR_STRIP_0, stripObj0);
    EEPROM.get(EEPROM_ADDR_STRIP_1, stripObj1);
    EEPROM.get(EEPROM_ADDR_STRIP_2, stripObj2);
    EEPROM.get(EEPROM_ADDR_PINS, pinObj);
    EEPROM.get(EEPROM_BRIGHTNESS, brightness);
    _brightness = (int)brightness;

    Serial.begin(9600);  

    strip2.begin();
    strip2.show();
    strip1.begin();
    strip1.show();
    strip0.begin();
    strip0.show();

    //regiser cloud variables and the run function
    Particle.function("run", run);
    Particle.variable("action", action, STRING);
    Particle.variable("parameters", parameters, STRING);
    Particle.variable("brightness", &_brightness, INT);

    pinMode(PIN_0, OUTPUT);
    pinMode(PIN_1, OUTPUT);
    pinMode(PIN_2, OUTPUT);
    pinMode(PIN_3, OUTPUT);
    pinMode(PIN_4, OUTPUT);
    pinMode(PIN_5, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    // if(String(stripObj2.params).equals("") || String(stripObj2.params).equals("INIT"))
    // {
    //     Serial.println("Reseting state");
    //     brightness = 255;
    //     _brightness = 255;
    //     EEPROM.put(EEPROM_BRIGHTNESS, 255);
    // }
    // else
    // {
    //     Serial.println("Resuming state: " + String(stripObj2.params));
    //     if(brightness > 0 && brightness < 256)
    //     {
    //         strip2.setBrightness(brightness);
    //         strip1.setBrightness(brightness);
    //         strip0.setBrightness(brightness);
    //     }
    //     run(String(stripObj2.params));
    // }

    for(int i=0; i<NUM_PINS; i++)
    {
        int pin = pinForId(i);
        digitalWrite(pin, pinObj.pins[i]);
    }

    //particle emitter default properties
    emitter.respawnOnOtherSide = false;
    emitter.threed = false;
    emitter.flicker = false;
    emitter.numParticles = 3;
    float mvf = 5 / 10.0;
    emitter.maxVelocity = mvf / FPS;

    Serial.println("Setup done.");
}

//wifi connect button was pushed
void connect() 
{
  Serial.println("Connecting to Wifi");
  if (Particle.connected() == false) 
  {
    Particle.connect();
  }
}

//mode button was pushed
void mode()
{
    Serial.println("Changing mode");
    // if(loopRun == BIKE1)
    //     loopRun = BIKE2;
    // else if(loopRun == BIKE2)
    //     loopRun = BIKE1;
}

void loop() 
{
    Serial.println("loop");
    if (Particle.connected() == false) 
    {
        Particle.connect();
    }

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
    else if(loopRun.equals(SETALL))
    {
        int r = stringToInt(loopArgs[1]);
        int g = stringToInt(loopArgs[2]);
        int b = stringToInt(loopArgs[3]);
        setAll(r, g, b);
        delay(1000);
    }
    else if(loopRun.equals(ALTERNATE))
    {
        int r1 = stringToInt(loopArgs[1]);
        int g1 = stringToInt(loopArgs[2]);
        int b1 = stringToInt(loopArgs[3]);
        int r2 = stringToInt(loopArgs[4]);
        int g2 = stringToInt(loopArgs[5]);
        int b2 = stringToInt(loopArgs[6]);
        staticAlternate(r1, g1, b1, r2, g2, b2);
        delay(1000);
    }
    else if(loopRun.equals(LOOPALTERNATE))
    {
        int r1 = stringToInt(loopArgs[1]);
        int g1 = stringToInt(loopArgs[2]);
        int b1 = stringToInt(loopArgs[3]);
        int r2 = stringToInt(loopArgs[4]);
        int g2 = stringToInt(loopArgs[5]);
        int b2 = stringToInt(loopArgs[6]);
        int d = stringToInt(loopArgs[7]);

        staticAlternate(r1, g1, b1, r2, g2, b2);
        delay(d);
        staticAlternate(r2, g2, b2, r1, g1, b1);
        delay(d);
    }
    else if(loopRun.equals(BLOCKS))
    {
        int r1 = stringToInt(loopArgs[1]);
        int g1 = stringToInt(loopArgs[2]);
        int b1 = stringToInt(loopArgs[3]);
        int r2 = stringToInt(loopArgs[4]);
        int g2 = stringToInt(loopArgs[5]);
        int b2 = stringToInt(loopArgs[6]);
        int blockSize = stringToInt(loopArgs[7]);
        buildBlocks(r1, g1, b1, r2, g2, b2, blockSize);
        delay(1000);
    }
    else if(loopRun.equals(LOOPBLOCKS))
    {
        int r1 = stringToInt(loopArgs[1]);
        int g1 = stringToInt(loopArgs[2]);
        int b1 = stringToInt(loopArgs[3]);
        int r2 = stringToInt(loopArgs[4]);
        int g2 = stringToInt(loopArgs[5]);
        int b2 = stringToInt(loopArgs[6]);
        int d = stringToInt(loopArgs[7]);
        int blockSize = stringToInt(loopArgs[8]);
        
        animateBlocks(r2, g2, b2, r1, g1, b1, blockSize, d, true);
        animateBlocks(r2, g2, b2, r1, g1, b1, blockSize, d, false);
    }
    else if(loopRun.equals(FADECOLOR))
    {
        int r1 = stringToInt(loopArgs[1]);
        int g1 = stringToInt(loopArgs[2]);
        int b1 = stringToInt(loopArgs[3]);
        int r2 = stringToInt(loopArgs[4]);
        int g2 = stringToInt(loopArgs[5]);
        int b2 = stringToInt(loopArgs[6]);
        int d = stringToInt(loopArgs[7]);
        int duration = stringToInt(loopArgs[8]);
        
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
        int r1 = stringToInt(loopArgs[1]);
        int g1 = stringToInt(loopArgs[2]);
        int b1 = stringToInt(loopArgs[3]);
        int r2 = stringToInt(loopArgs[4]);
        int g2 = stringToInt(loopArgs[5]);
        int b2 = stringToInt(loopArgs[6]);
        int d = stringToInt(loopArgs[7]);
        endRun(r1, g1, b1, r2, g2, b2, d);
    }
    else if(loopRun.equals(SNOW))
    {
        snow();
    }
    else if(loopRun.equals(USA))
    {
        runUSA();
    }
    else if(loopRun.equals(LIGHTNING))
    {
        runLightning();
    }
    else if(loopRun.equals(BIKE1))
    {
        Serial.println("BIKE1");
        strip0.setBrightness(255);
        strip1.setBrightness(128);
        strip2.setBrightness(255);
        // allOff();
        bikeRightTail(strip1.Color(255,0,0));
        bikeRightSide(strip1.Color(255,165,0));

        bikeLeftTail(strip1.Color(255,0,0));
        bikeLeftSide(strip1.Color(255,165,0));

        bikeMiddle(strip1.Color(76,0,153));

        bikeHood(strip1.Color(0,255,0));

        
        delay(2000);
        // bikeRightTail(strip1.Color(0,0,0));
        // bikeLeftTail(strip1.Color(0,0,0));
        // bikeLeftSide(strip1.Color(0,0,0));
        // bikeRightSide(strip1.Color(0,0,0));
        // delay(1000);
    }
    else if(loopRun.equals(BIKE2))
    }
        strip0.setBrightness(255);
        strip1.setBrightness(128);
        strip2.setBrightness(255);

        bikeMiddle(strip0.Color(255,0,255));
        bikeHood(strip0.Color(0,255,0));

        for(int x=0; x<500; x++)
        {
            uint32_t color = strip1.Color(random(255), 56, random(255));

            for(int i=0; i<BIKE_LEFT_SIDE_END; i+=3)
            {
                setStrip1(0,0,0);
                // strip1.show();
                int j = BIKE_RIGHT_TAIL_END - i;
                strip1.setPixelColor(i, color);
                strip1.setPixelColor(i+1, color);
                strip1.setPixelColor(i+2, color);
                strip1.setPixelColor(j, color);
                strip1.setPixelColor(j-1, color);
                strip1.setPixelColor(j-2, color);
                strip1.show();
                delay(40);
            }    
        }

        for(int i=0; i<1000; i++)
        {
            particles();
        }

        for(int i=0; i<10; i++)
        {
            rainbow(20);
        }
    }
}

void bikeHood(uint32_t color)
{
    for(int i=BIKE_HOOD_START; i<=BIKE_MIDDLE_END; i++)
    {
        strip2.setPixelColor(i, color);
    }
    strip2.show();
}

void bikeMiddle(uint32_t color)
{
    for(int i=BIKE_MIDDLE_START; i<=BIKE_MIDDLE_END; i++)
    {
        strip0.setPixelColor(i, color);
    }
    strip0.show();
}

void bikeLeftTail(uint32_t color)
{
    for(int i=BIKE_LEFT_TAIL_START; i<=BIKE_LEFT_TAIL_END; i++)
    {
        strip1.setPixelColor(i, color);
    }
    strip1.show();
}

void bikeRightTail(uint32_t color)
{
    for(int i=BIKE_RIGHT_TAIL_START; i<=BIKE_RIGHT_TAIL_END; i++)
    {
        strip1.setPixelColor(i, color);
    }
    strip1.show();
}

void bikeLeftSide(uint32_t color)
{
    for(int i=BIKE_LEFT_SIDE_START; i<=BIKE_LEFT_SIDE_END; i++)
    {
        strip1.setPixelColor(i, color);
    }
    strip1.show();
}

void bikeRightSide(uint32_t color)
{
    for(int i=BIKE_RIGHT_SIDE_START; i<=BIKE_RIGHT_SIDE_END; i++)
    {
        strip1.setPixelColor(i, color);
    }
    strip1.show();
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
    args = stringSplit(params, ',');
    String command = args[0];

    if(!command.equals(ON) && !command.equals(OFF) && !command.equals(SETBRIGHTNESS))
    { //save the pin commands
        params.toCharArray(stripObj2.params, 64);
        EEPROM.put(EEPROM_ADDR_STRIP_2, stripObj2);
        strcpy(parameters, params.c_str());
        strcpy(action, command.c_str());
        // loopArgs = args;
        for(int i=0; i<NUM_ARGS; i++)
        {
            loopArgs[i] = args[i];
        }
    }

    if(command.equals(ALLOFF))
    {
        loopRun = SHUTDOWN;
    }
    else if(command.equals(SETALL))
    {
        loopRun = SETALL;
    }
    else if(command.equals(ALTERNATE))
    {
        loopRun = ALTERNATE;
    }
    else if(command.equals(LOOPALTERNATE))
    {
        loopRun = LOOPALTERNATE;
    }
    else if(command.equals(LOOPBLOCKS))
    {
        loopRun = LOOPBLOCKS;
    }
    else if(command.equals(BLOCKS))
    {
        loopRun = BLOCKS;
    }
    else if(command.equals(FADECOLOR))
    {
        loopRun = FADECOLOR;
    }
    else if(command.equals(RAINBOW))
    {
        loopRun = RAINBOW;
    }
    else if(command.equals(STOP))
    {
        loopRun = STOP;
    }
    else if(command.equals(SHUTDOWN))
    {
        loopRun = SHUTDOWN;
    }
    else if(command.equals(ENDRUN))
    {
        loopRun = ENDRUN;
    }
    else if(command.equals(SNOW))
    {
        loopRun = SNOW;
    }
    else if(command.equals(PARTICLES))
    {
        loopRun = PARTICLES;
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
    }
    else if(command.equals(USA))
    {
        loopRun = USA;
    }
    else if(command.equals(LIGHTNING))
    {
        loopRun = LIGHTNING;
    }
    else if(command.equals(SETBRIGHTNESS))
    {
        brightness = stringToInt(args[1]);
        _brightness = (int)brightness;
        EEPROM.put(EEPROM_BRIGHTNESS, brightness);
        strip2.setBrightness(brightness);
        strip2.show();
    }
    else if(command.equals(ON))
    {
        setPinState(ON, args);
    }
    else if(command.equals(OFF))
    {
        setPinState(OFF, args);
    }
    else 
    { //command not found
        return 0;
    }
    return 1;
}

void setPinState(String command, String* args)
{
    int pinState = LOW;
    if(command.equals(ON))
    {
        pinState = HIGH;
    }

    int pinNum = stringToInt(args[1]);
    pinObj.pins[pinNum] = pinState;
    int pin = pinForId(pinNum);
    digitalWrite(pin, pinState);    
    EEPROM.put(EEPROM_ADDR_PINS, pinObj);
}

int pinForId(int id)
{
    switch(id)
    {
        case 0 : return PIN_0;
        case 1 : return PIN_1;
        case 2 : return PIN_2;
        case 3 : return PIN_3;
        case 4 : return PIN_4;
        case 5 : return PIN_5;
        default : return -1;
    }
}

int runLightning()
{
    setAll(255,255,255);
    delay(random(100));
    setAll(0,0,0);
    delay(random(100));
    setAll(255,255,255);
    delay(random(100));
    setAll(0,0,0);
    delay(random(100));
    setAll(255,255,255);
    delay(random(100));
    setAll(0,0,0);
    delay(random(100));
    setAll(255,255,255);
    delay(random(100));
    setAll(0,0,0);
    delay(random(10000));
    
    return 1;
}

int runUSA()
{
    int blockSize = 5;
    int j = 0;
    {
        int count = -1;
        for(int i=0; i<strip2.numPixels(); i++) {
            if(i % blockSize == 0)
            {
                count++;
                if(count > 2)
                    count = 0;
            }
            
            int pix = 0;
            if( (i + j) >= strip2.numPixels())
            {
                pix = (i + j) - strip2.numPixels();
            }
            else 
            {
                pix = i + j;
            }
            
            if(count == 0)
            {
                strip2.setPixelColor(pix, strip2.Color(255, 0, 0));
            }
            else if(count == 1)
            {
                strip2.setPixelColor(pix, strip2.Color(255, 255, 255));
            }
            else if(count == 2)
            {
                strip2.setPixelColor(pix, strip2.Color(0, 0, 255));
            }
            
        }
        strip2.show();
        delay(500);
    }
    return 1;
}

int snow()
{
    for(int i=0; i<strip2.numPixels() / 10; i++)
    { //pick the random pixels
        int pix = random(strip2.numPixels());
        strip2.setPixelColor(pix, strip2.Color(0, 0, 0));
        strip2.show();
        delay(random(50));
    }
    
    for(int i=0; i<strip2.numPixels() / 10; i++)
    { //pick the random pixels
        int pix = random(strip2.numPixels());
        int brightness = random(255);
        strip2.setPixelColor(pix, strip2.Color(brightness, brightness, brightness * .20)); //poor mans white balance
        strip2.show();
        delay(random(50));
    }
}

int endRun(uint8_t r1, uint8_t g1, uint8_t b1, 
    uint8_t r2, uint8_t g2, uint8_t b2,
    uint8_t d)
{
    for(int i=0; i<strip2.numPixels(); i++)
    {
        strip2.setPixelColor(i, strip2.Color(r1, g1, b1));
        strip2.setPixelColor(strip2.numPixels() - i, strip2.Color(r2, g2, b2));
        strip2.show();
        delay(d);
    }
}

int animateBlocks(uint8_t r1, uint8_t g1, uint8_t b1, 
    uint8_t r2, uint8_t g2, uint8_t b2, uint8_t blockSize, int d, bool inBlock)
{
    for(int j=0; j<blockSize; j++)
    {
        for(int i=0; i<strip2.numPixels(); i++) {
            if(i % blockSize == 0)
            {
                inBlock = !inBlock;
            }
            
            int pix = 0;
            if( (i + j) >= strip2.numPixels())
            {
                pix = (i + j) - strip2.numPixels();
            }
            else 
            {
                pix = i + j;
            }
            
            if(inBlock)
            {
                strip2.setPixelColor(pix, strip2.Color(r1, g1, b1));
            }
            else
            {
                strip2.setPixelColor(pix, strip2.Color(r2, g2, b2));
            }
        }
        strip2.show();
        delay(d);
    }
    
    return 1;
}

int buildBlocks(uint8_t r1, uint8_t g1, uint8_t b1, 
    uint8_t r2, uint8_t g2, uint8_t b2,
    uint8_t blockSize)
{               
    bool inBlock = true;
    
    for(int i=0; i<strip2.numPixels(); i++) 
    {
        if(i % blockSize == 0)
        {
            inBlock = !inBlock;
        }
        
        if(inBlock)
        {
            strip2.setPixelColor(i, strip2.Color(r1, g1, b1));
        }
        else
        {
            strip2.setPixelColor(i, strip2.Color(r2, g2, b2));
        }
    }
    strip2.show();
    return 1;
}

int staticAlternate(uint8_t r1, uint8_t g1, uint8_t b1, 
    uint8_t r2, uint8_t g2, uint8_t b2)
{
    for(int i=0; i<strip2.numPixels(); i++) 
    {
        if(i % 2 == 0)
        {
            strip2.setPixelColor(i, strip2.Color(r1, g1, b1));
        }
        else
        {
            strip2.setPixelColor(i, strip2.Color(r2, g2, b2));
        }
    }
    strip2.show();
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
    setStrip0(r, g, b);
    setStrip1(r, g, b);
    setStrip2(r, g, b);
    return 1;
}

void setStrip0(uint8_t r, uint8_t g, uint8_t b)
{
    for(int i=0; i<strip0.numPixels(); i++) 
    {
      strip0.setPixelColor(i, strip0.Color(r, g, b));
    }
    strip0.show();
}

void setStrip1(uint8_t r, uint8_t g, uint8_t b)
{
    for(int i=1; i<strip1.numPixels(); i++) 
    {
      strip1.setPixelColor(i, strip1.Color(r, g, b));
    }
    strip1.show();
}

void setStrip2(uint8_t r, uint8_t g, uint8_t b)
{
    for(int i=2; i<strip2.numPixels(); i++) 
    {
      strip2.setPixelColor(i, strip2.Color(r, g, b));
    }
    strip2.show();
}

int rainbow(int d) {
    uint16_t i, j;

    for(j=0; j<256; j++) {
    for(i=0; i<strip0.numPixels(); i++) 
    {
          strip0.setPixelColor(i, Wheel((i+j) & MAX_COLOR));
    }
    for(i=0; i<strip1.numPixels(); i++) 
    {
          strip1.setPixelColor(i, Wheel((i+j) & MAX_COLOR));
    }
    for(i=0; i<strip2.numPixels(); i++) 
    {
          strip2.setPixelColor(i, Wheel((i+j) & MAX_COLOR));
    }
    strip2.show();
    strip1.show();
    strip0.show();
    delay(d);
    }
    return 1;
}

// Input a value 0 to MAX_COLOR to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
    int maxVal = MAX_COLOR;
    if(WheelPos < 85) 
    {
     return strip2.Color(WheelPos * 3, maxVal - WheelPos * 3, 0);
    } 
    else if(WheelPos < 170) 
    {
     WheelPos -= 85;
     return strip2.Color(maxVal - WheelPos * 3, 0, WheelPos * 3);
    } 
    else 
    {
     WheelPos -= 170;
     return strip2.Color(0, WheelPos * 3, maxVal - WheelPos * 3);
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

    for (int16_t i = steps; i >= 0; i--) 
    {
        redValue = r1 + (redDiff * i / steps);
        greenValue = g1 + (greenDiff * i / steps);
        blueValue = b1 + (blueDiff * i / steps);

        for (uint16_t i = 0; i < strip2.numPixels(); i++) 
        {
            strip2.setPixelColor(i, strip2.Color(redValue, greenValue, blueValue));
        }
        strip2.show();
        delay(del);
    }

    return 1;
}

void particles() 
{
    unsigned long frameStartMillis = millis();
    emitter.stripPosition = 0.5; //random(100) / 100.0;

    // Draw each particle
    for (int i=0; i < emitter.numParticles; i++) 
    {
        // Update this particle's position
        LEDParticle prt = emitter.updateParticle(i);

        float zScale = (emitter.threed ? (1.0 - prt.coord.z) : 1.0);
        double tailLength = (1.0 + abs(prt.velocity.x * 15) * zScale);
        int16_t startSlot = emitter.numPixels * prt.coord.x;
        int16_t currentSlot = startSlot;
        int16_t oldSlot = currentSlot;

        // Draw the particle and its tail
        // High velocity particles have longer tails
        for (int ii=0; ii < tailLength; ii++) 
        {
            // Taper the tail fade
            float colorScale = ((tailLength - (ii * GOLDEN_RATIO)) / tailLength);
            if (ii == 0 && prt.dimmed) 
            {// Flicker the first particle
                colorScale *= (random(50) / 100) + 0.05;
            }      

            if (emitter.threed) 
            {
                colorScale *= zScale;
            }

            if (colorScale < 0.05) 
            {
                colorScale = 0.0;
            }

            // Draw particle
            // strip2.setPixelColor(currentSlot, 
            //                     strip2.Color(prt.redColor * colorScale, 
            //                                 prt.greenColor * colorScale, 
            //                                 prt.blueColor * colorScale));
            strip1.setPixelColor(currentSlot, 
                                strip2.Color(prt.redColor * colorScale, 
                                            prt.greenColor * colorScale, 
                                            prt.blueColor * colorScale));
            // strip0.setPixelColor(currentSlot, 
            //                     strip2.Color(prt.redColor * colorScale, 
            //                                 prt.greenColor * colorScale, 
            //                                 prt.blueColor * colorScale));

            oldSlot = currentSlot;
            currentSlot = startSlot + ((ii+1) * (prt.velocity.x > 0 ? -1 : 1));
        }

        //Terminate the tail
        // strip2.setPixelColor(oldSlot, strip2.Color(0, 0, 0));
        strip1.setPixelColor(oldSlot, strip1.Color(0, 0, 0));
        // strip0.setPixelColor(oldSlot, strip2.Color(0, 0, 0));
    }

    uint16_t frameElapsedMillis = millis() - frameStartMillis;
    uint16_t frameDelayMillis = (MILLIS_PER_FRAME - frameElapsedMillis);

    if (frameDelayMillis > 0.0) 
    {
        Serial.println(frameDelayMillis);
        delay(frameDelayMillis);
        // strip2.show();
        strip1.show();
        // strip0.show();
    }
}

void setCoordColor(Coord3D coord, uint32_t color) 
{
    // strip2.setPixelColor(coord.x * emitter.numPixels, color); 
    strip1.setPixelColor(coord.x * emitter.numPixels, color); 
    // strip0.setPixelColor(coord.x * emitter.numPixels, color); 
}

