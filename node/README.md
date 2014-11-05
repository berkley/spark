This server provides access to one or more spark core processors via the spark.io API

* Port: 3000

You'll need to change the config.json file to match your core ids and your access_token.  You can also pass the access_token via the command line if you don't want to put it in a text file.  

Configuration
=============
```
{
    "access_token": "123456789",
    "port": 3000,
    "spark_url":"https://api.spark.io/v1/devices",
    "cores": [
        {
            "name": "Outside",
            "id": "12345678",
            "description": "Outside of the house"
        },
        {
            "name": "Sidewalk",
            "id": "987654321",
            "description": "Strip along the sidewalk"
        }
    ]
}
```

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

In a browser
============
```
http://localhost:3000
```

