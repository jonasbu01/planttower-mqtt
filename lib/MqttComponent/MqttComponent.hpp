#ifndef MQTT_COMPONENT
#define MQTT_COMPONENT

#include <map>
#include <ArduinoJson.h>
#include "MqttEntity.hpp"
#include "PersistentSettings.hpp"

class MqttComponent : public MqttEntity {
 public:
  MqttComponent(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name,
    const char* platform,
    std::map<std::string, std::string>* additional_discovery_config = new std::map<std::string, std::string>()
  ): MqttEntity(mqtt_client, unique_id, name, platform) {
    this->additional_discovery_config = additional_discovery_config;
  }

  virtual void append_discovery_config(JsonObject* config) = 0;
  virtual void load_persistent_settings() {};

  virtual ~MqttComponent() {
    delete this->additional_discovery_config;
  }

 protected:
  std::map<std::string, std::string>* additional_discovery_config;

  void append_additional_discovery_config(JsonObject* config) {
    for (auto const& pair : *this->additional_discovery_config) {
      (*config)[pair.first.c_str()] = pair.second.c_str();
    }
  }
};

#endif