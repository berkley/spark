spark
=====
Spark.io and Node.js controlled home automation (currently: LED lighting and watering system)

Overview
========
Integrated home automation controls.  This app and firmware currently have two modules: 1) LED pixel controls 2) Solenoid based sprinkler system controls

* Control neopixel LED strips connected to one or more Spark Core (http://spark.io) processors with a web interface.  You can use this to control various parts of your house lighting, an art project or whatever else you want to program.  Use the /lights route on the node app to control lighting.  

* Control your watering system.  Configure pins D0-D7 to control solenoid valves via transistors or relays.  

You'll need to flash each of your cores with the appropriate firmware, then run the node.js server on a computer with access to the internet.  

See the README in each subdirectory for a more detailed description of its contents.