#include "application.h"

//pins that control diferent watering zones or monitor different doors
#define PIN_0 D0
#define PIN_1 D1 //front yard
#define PIN_2 D2 //garden
#define PIN_3 D3 //side yard
#define PIN_4 D4 //front door
#define PIN_5 D5 //back door
#define PIN_6 D6 //basement door
#define PIN_7 D7

int ledPin = D7;
int frontDoorValue = 0;
int backDoorValue = 0;
int basementDoorValue = 0;

const char* frontDoorName = "frontdoor";
const char* backDoorName = "backdoor";
const char* basementDoorName = "basedoor";

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
    pinMode(PIN_4, INPUT_PULLDOWN);
    pinMode(PIN_5, INPUT_PULLDOWN);
    pinMode(PIN_6, INPUT_PULLDOWN);
    pinMode(ledPin, OUTPUT);

    for(int i=0; i<8; i++)
    {
        int pin = pinForId(i);
        digitalWrite(pin, LOW); //init all off
    }

    //register the run command as an API endpoint
    Spark.function("run", run);
    //register the action variable as a GET parameter
    Spark.variable("action", &action, STRING);
    //register the parameters variable as a GET parameter
    Spark.variable("parameters", &parameters, STRING);

    Spark.variable(frontDoorName, &frontDoorValue, INT);
    Spark.variable(backDoorName, &backDoorValue, INT);
    Spark.variable(basementDoorName, &basementDoorValue, INT);
}

void loop() 
{
    pinVal(PIN_4, &frontDoorValue, frontDoorName);
    pinVal(PIN_5, &backDoorValue, backDoorName);
    pinVal(PIN_6, &basementDoorValue, basementDoorName);
    delay(1000);
    // Spark.publish("loop", "1", 60, PRIVATE);
}

void pinVal(int pin, int* pinValue, String pinName)
{
//   Spark.publish("pinRead", "1", 60, PRIVATE);
  if(digitalRead(pin) == HIGH)
  {
    if(*pinValue == 0)
    {
      Spark.publish(pinName,"Closed", 60, PRIVATE);
      *pinValue = 1; //door is closed
      digitalWrite(ledPin, LOW);
    }
  }
  else
  {
    if(*pinValue == 1)
    {
      Spark.publish(pinName,"Open", 60, PRIVATE);
      *pinValue = 0; //door is open
      digitalWrite(ledPin, HIGH);
    }
  }
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
