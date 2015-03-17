TCPClient client;

#define SOCKET_PORT 30001

// byte server[] = {10, 0, 1, 6}; //Dino
IPAddress server(10, 0, 1, 8);


void setup()
{
  Serial.begin(9600);
  connect();
}

void loop()
{
  if (!client.connected())
  {
    Serial.println("closing and reconnecting....");
    // client.stop();
    connect();
    delay(1000);
  }
  else
  {
    int available_bytes = client.available();
    if(available_bytes)
    {
      // char *buffer = (char*)malloc((available_bytes + 1) * sizeof(char) );
      // int count = 0;
      // char c = client.read();
      // while(c != -1)
      // {
      //   buffer[count] = c;
      //   c = client.read();
      //   count++;
      // }
      // client.flush();
      // Serial.print("Data received: ");
      // for(int i=0; i<available_bytes; i++)
      // {
      //   Serial.print(buffer[i]);
      // }
      // Serial.println("//DONE//");
      // free(buffer);

      // client.println("ack");  
      char c = client.read();
      client.flush();
      Serial.print("received ");
      Serial.println(c);
      client.println("ack");
    }
  }

   
}

void connect()
{
  Serial.print("connecting to ");
  Serial.print(server[0]);
  Serial.print(".");
  Serial.print(server[1]);
  Serial.print(".");
  Serial.print(server[2]);
  Serial.print(".");
  Serial.print(server[3]);
  Serial.print(" from ");
  Serial.println(WiFi.localIP());

  if(client.connect(server, SOCKET_PORT))
  {
    Serial.println("connected");
    client.print("ident ");
    client.println(WiFi.localIP());
  }
  else
  {
    Serial.println("connection failed");
  }
}