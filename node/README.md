This server provides access to WS2811/12 neopixel LED strips connected to one or more spark core processors via the spark.io API

It also provides access to the sprinkler system controls in the water-controller firmware.  

You'll need to change the config.json file to match your core ids and your access_token.  You can also pass the access_token via the command line if you don't want to put it in a text file.  

Configuration
=============
```
{
    "access_token": "PUT YOUR ACCESS TOKEN HERE",
    "port": 3000,
    "sendDelay":300,
    "defaultBrightness":32,
    "spark_url":"https://api.spark.io/v1/devices",
    "cores": [
        {
            "name": "Core1",
            "id": "PUT THE DEVICE ID HERE",
            "description": "This is my Spark MCU."
        },
        {
            "name": "Core2",
            "id": "PUT THE DEVICE ID HERE",
            "description": "This is my Spark MCU."
        }
    ],
    "water":{"core":"Core1",
             "valves":[{"id":"1", "name":"Front Yard"},
                       {"id":"2", "name":"Garden"}]}
}
```
For the watering app, the valve id must match the pin number that you have your valve connected to on the core.  See the firmware README for more information.

Installation
============
```
npm install
```

Startup
=======
```
npm start
or
node app.js --access_token 1234567890
```

Web interface
============
```
http://localhost:3000
```

