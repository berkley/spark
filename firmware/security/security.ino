int boardLed = D7;
int basementDoorPin = D0;

int basementDoorValue = 0;

void setup()
{
  pinMode(basementDoorPin, INPUT_PULLDOWN);
  pinMode(boardLed, OUTPUT);
  Spark.variable("basementDoorValue", &basementDoorValue, INT);
}

void loop()
{
  if(digitalRead(basementDoorPin) == HIGH)
  {
    if(basementDoorValue == 0)
    {
      Spark.publish("basementDoor","closed", 60, PRIVATE);
      basementDoorValue = 1; //door is closed
      digitalWrite(boardLed, LOW);
    }
  }
  else
  {
    if(basementDoorValue == 1)
    {
      Spark.publish("basementDoor","open", 60, PRIVATE);
      basementDoorValue = 0; //door is open
      digitalWrite(boardLed, HIGH);
    }
  }
  delay(1000);
}
