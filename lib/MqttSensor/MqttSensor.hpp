#ifndef MQTTSENSOR
#define MQTTSENSOR

#include <ArduinoJson.h>
#include <PubSubClient.h>

#include "Arduino.h"

class MqttSensor {
 private:
  PubSubClient* client;
  const char* name;
  const char* unique_id;
  const char* device_class;
  const char* unit;
  const char* value_template;
  char state_topic[256];
  
 public:
  MqttSensor(
    PubSubClient* client,
    const char* unique_id,
    const char* name,
    const char* device_class,
    const char* unit,
    const char* value_template
  ): client(client),
    unique_id(unique_id),
    name(name),
    device_class(device_class),
    unit(unit),
    value_template(value_template) {
      snprintf(this->state_topic, 256, "homeassistant/sensor/%s/state", unique_id);
    }

  void send_state();
  const char* get_unique_id();
  void append_discovery_config(JsonObject* config);
};

#endif