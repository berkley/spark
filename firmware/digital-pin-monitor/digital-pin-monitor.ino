int ledPin = D7;

int pin0 = A0;
int pin1 = D1;
int pin2 = D2;
int pin3 = D3;
int pin4 = D4;
int pin5 = D5;
int pin6 = D6;

int pin0Value = 0;
int pin1Value = 0;
int pin2Value = 0; 
int pin3Value = 0;
int pin4Value = 0;
int pin5Value = 0;
int pin6Value = 0;

const char* pin0Name = "pin0Value";
const char* pin1Name = "pin1Value";
const char* pin2Name = "pin2Value";
const char* pin3Name = "pin3Value";
const char* pin4Name = "pin4Value";
const char* pin5Name = "pin5Value";
const char* pin6Name = "pin6Value";

void setup()
{
  pinMode(pin0, INPUT_PULLDOWN);
  pinMode(pin1, INPUT_PULLDOWN);
  pinMode(pin2, INPUT_PULLDOWN);
  pinMode(pin3, INPUT_PULLDOWN);
  pinMode(pin4, INPUT_PULLDOWN);
  pinMode(pin5, INPUT_PULLDOWN);
  pinMode(pin6, INPUT_PULLDOWN);

  pinMode(ledPin, OUTPUT);

  Spark.variable(pin0Name, &pin0Value, INT);
  Spark.variable(pin1Name, &pin1Value, INT);
  Spark.variable(pin2Name, &pin2Value, INT);
  Spark.variable(pin3Name, &pin3Value, INT);
  Spark.variable(pin4Name, &pin4Value, INT);
  Spark.variable(pin5Name, &pin5Value, INT);
  Spark.variable(pin6Name, &pin6Value, INT);
}

void loop()
{
  pinVal(pin0, &pin0Value, pin0Name);
  pinVal(pin1, &pin1Value, pin1Name);
  pinVal(pin2, &pin2Value, pin2Name);
  pinVal(pin3, &pin3Value, pin3Name);
  pinVal(pin4, &pin4Value, pin4Name);
  pinVal(pin5, &pin5Value, pin5Name);
  pinVal(pin6, &pin6Value, pin6Name);
//   delay(1000);
}

void pinVal(int pin, int* pinValue, String pinName)
{
  if(digitalRead(pin) == HIGH)
  {
    if(*pinValue == 0)
    {
      Spark.publish(pinName,"HIGH", 60, PRIVATE);
      *pinValue = 1; //door is closed
      digitalWrite(ledPin, LOW);
    }
  }
  else
  {
    if(*pinValue == 1)
    {
      Spark.publish(pinName,"LOW", 60, PRIVATE);
      *pinValue = 0; //door is open
      digitalWrite(ledPin, HIGH);
    }
  }
}
