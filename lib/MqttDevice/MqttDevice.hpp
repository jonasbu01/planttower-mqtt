#ifndef MQTT_DEVICE
#define MQTT_DEVICE

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <vector>
#include "Arduino.h"
#include "MqttEntity.hpp"
#include "MqttComponent.hpp"

class MqttDevice : public MqttEntity {
 private:
  const char* manufacturer;
  char discovery_topic[256];
  std::vector<MqttComponent*>* components;

 public:
  MqttDevice(
    PubSubClient* client,
    const char* unique_id,
    const char* name,
    const char* manufacturer,
    std::vector<MqttComponent*>* components
  ): MqttEntity(client, unique_id, name, "device"),
    manufacturer(manufacturer),
    components(components) {
      snprintf(this->discovery_topic, 256, "homeassistant/%s/%s/config", this->platform, unique_id);
    }

  void send_discovery();
};

#endif