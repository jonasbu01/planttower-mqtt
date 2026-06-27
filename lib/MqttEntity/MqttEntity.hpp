#ifndef MQTT_ENTITY
#define MQTT_ENTITY

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"

class MqttEntity {
 public:
  MqttEntity(
    PubSubClient* mqtt_client,
    const char* unique_id_value,
    const char* name_value,
    const char* platform_value
  ): mqtt_client(mqtt_client) {
      snprintf(this->unique_id, sizeof(this->unique_id), "%s", unique_id_value ? unique_id_value : "");
      snprintf(this->name, sizeof(this->name), "%s", name_value ? name_value : "");
      snprintf(this->platform, sizeof(this->platform), "%s", platform_value ? platform_value : "");
      snprintf(this->state_topic, sizeof(this->state_topic), "homeassistant/%s/%s/state", this->platform, this->unique_id);
    }

  const char* get_unique_id();
  const char* get_state_topic();

 protected:
  PubSubClient* mqtt_client;
  char name[128];
  char unique_id[128];
  char platform[64];
  char state_topic[256];
};

#endif