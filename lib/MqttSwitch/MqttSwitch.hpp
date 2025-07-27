#ifndef MQTT_SWITCH
#define MQTT_SWITCH

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "MqttStatefulComponent.hpp"

class MqttSwitch : public MqttStatefulComponent<const char*> {
 private:
  char command_topic[256];

 protected:
  void serialize_state(char* serialized_state_buffer, size_t buffer_size) override;

 public:
  static constexpr const char* ON_STATE = "ON";
  static constexpr const char* OFF_STATE = "OFF";

  MqttSwitch(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name
  ): MqttStatefulComponent<const char*>(mqtt_client, unique_id, name, "switch") {
      snprintf(this->command_topic, 256, "homeassistant/%s/%s/set", platform, unique_id);
    }

  void append_discovery_config(JsonObject* config) override;

  void switch_on();
  void switch_off();
  void toggle();
};

#endif