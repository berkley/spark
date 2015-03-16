TCPClient client;
byte server[] = {10, 0, 1, 6}; //Dino

void setup()
{
  Serial.begin(9600);
  // Now open your Serial Terminal, and hit any key to continue!
  while(!Serial.available()) SPARK_WLAN_Loop();

  connect();
}

void loop()
{
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
    client.println("ack");  
  }

  if (!client.connected())
  {
    Serial.println("reconnecting");
    connect();
  }
}

void connect()
{
  Serial.println("connecting...");

  if(client.connect(server, 3002))
  {
    Serial.println("connected");
    client.print("ident: ");
    client.println(WiFi.localIP());
  }
  else
  {
    Serial.println("connection failed");
  }
}