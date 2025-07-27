#ifndef MQTT_COMPONENT
#define MQTT_COMPONENT

#include <ArduinoJson.h>
#include "MqttEntity.hpp"

class MqttComponent : public MqttEntity {
 public:
  MqttComponent(
    PubSubClient* client,
    const char* unique_id,
    const char* name,
    const char* platform
  ): MqttEntity(client, unique_id, name, platform) {}
  virtual void append_discovery_config(JsonObject* config) = 0;
};

#endif