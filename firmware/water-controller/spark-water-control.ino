#include "application.h"

//pins that control diferent watering zones
#define PIN_0 D0
#define PIN_1 D1 //front yard
#define PIN_2 D2 //garden
#define PIN_3 D3
#define PIN_4 D4
#define PIN_5 D5
#define PIN_6 D6
#define PIN_7 D7

char action[64];
char parameters[64];

#define ON "on" 
#define OFF "off"

String *loopArgs = new String[20];
String *strArr = new String[20];

void setup() 
{
    //Serial.begin(9600);

    pinMode(PIN_0, OUTPUT);
    pinMode(PIN_1, OUTPUT);
    pinMode(PIN_2, OUTPUT);
    pinMode(PIN_3, OUTPUT);
    pinMode(PIN_4, OUTPUT);
    pinMode(PIN_5, OUTPUT);
    pinMode(PIN_6, OUTPUT);
    pinMode(PIN_7, OUTPUT);

    for(int i=0; i<8; i++)
    {
        int pin = pinForId(i);
        digitalWrite(pin, LOW); //init all off
    }

    //register the run command as an API endpoint
    Particle.function("run", run);
    //register the action variable as a GET parameter
    Particle.variable("action", &action, STRING);
    //retister the parameters variable as a GET parameter
    Particle.variable("parameters", &parameters, STRING);
}

void loop() 
{
    
}

/*
    This function handles the API requests to /run.  The params are a 
    comma separated list.  
    params format is <command>,<param0>,<param1>,...,<paramN>
    where <command> is the action to be run and <paramN> are the parameters that an individual
    action needs.
*/
int run(String params)
{
    String* args = stringSplit(params, ',');
    String command = args[0];
    strcpy(parameters, params.c_str());
    strcpy(action, command.c_str());
    int pinState;

    if(command.equals(ON))
    {
        pinState = HIGH;
    }
    else if(command.equals(OFF))
    {
        pinState = LOW;   
    }
    else 
    { //command not found
        return 0;
    }

    if(args[1].equals("all"))
    {
        for(int i=0; i<8; i++)
        {
            int pin = pinForId(i);
            digitalWrite(pin, pinState);         
        }
    }
    else
    {
        int pinNum = stringToInt(args[1]);
        int pin = pinForId(pinNum);
        digitalWrite(pin, pinState);    
    }
    return 1;
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
        case 6 : return PIN_6;
        case 7 : return PIN_7;
        default : return -1;
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