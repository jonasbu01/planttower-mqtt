#ifndef MQTT_ENTITY
#define MQTT_ENTITY

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"

class MqttEntity {
 protected:
  PubSubClient* client;
  const char* name;
  const char* unique_id;
  const char* platform;
  char state_topic[256];

 public:
  MqttEntity(
    PubSubClient* client,
    const char* unique_id,
    const char* name,
    const char* platform
  ): client(client),
    unique_id(unique_id),
    name(name),
    platform(platform) {
      snprintf(this->state_topic, 256, "homeassistant/%s/%s/state", this->platform, unique_id);
    }

  const char* get_unique_id();
};

#endif