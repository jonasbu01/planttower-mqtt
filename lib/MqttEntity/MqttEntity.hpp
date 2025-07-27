#ifndef MQTT_ENTITY
#define MQTT_ENTITY

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"

class MqttEntity {
 public:
  MqttEntity(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name,
    const char* platform
  ): mqtt_client(mqtt_client),
    unique_id(unique_id),
    name(name),
    platform(platform) {
      snprintf(this->state_topic, 256, "homeassistant/%s/%s/state", this->platform, unique_id);
    }

  const char* get_unique_id();
  const char* get_state_topic();

 protected:
  PubSubClient* mqtt_client;
  const char* name;
  const char* unique_id;
  const char* platform;
  char state_topic[256];
};

#endif