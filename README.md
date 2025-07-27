# Plant Tower MQTT

## Getting started
Create a file named Secrets.hpp in folder `lib/Secrets` of the following format:
```cpp
// Wifi
const char *ssid = "";
const char *password = "";

// MQTT
const char *mqtt_server = "<IP address of MQTT server>";
const int mqtt_port = <MQTT port>;
const char *mqtt_user = "";
const char *mqtt_pass = "";
```