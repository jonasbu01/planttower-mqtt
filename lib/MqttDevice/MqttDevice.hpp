#ifndef MQTTDEVICE
#define MQTTDEVICE

#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <vector>

#include "Arduino.h"
#include "MqttSensor.hpp"

class MqttDevice {
 private:
  PubSubClient* client;
  const char* unique_id;
  const char* name;
  const char* manufacturer;
  char discovery_topic[256];
  char state_topic[256];
  std::vector<MqttSensor>* components;

 public:
  MqttDevice(
    PubSubClient* client,
    const char* unique_id,
    const char* name,
    const char* manufacturer,
    std::vector<MqttSensor>* components
  ): client(client),
    unique_id(unique_id),
    name(name),
    manufacturer(manufacturer),
    components(components) {
      snprintf(this->discovery_topic, 256, "homeassistant/device/%s/config", unique_id);
      snprintf(this->state_topic, 256, "homeassistant/device/%s/state", unique_id);
    }
  void send_discovery();
};

#endif