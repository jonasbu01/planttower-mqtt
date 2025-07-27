#ifndef MQTT_SWITCH
#define MQTT_SWITCH

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "MqttControl.hpp"

class MqttSwitch : public MqttControl<const char*> {
 public:
  static constexpr const char* ON_STATE = "ON";
  static constexpr const char* OFF_STATE = "OFF";

  MqttSwitch(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name
  ): MqttControl<const char*>(mqtt_client, unique_id, name, "switch") {
    this->state = MqttSwitch::OFF_STATE;
  }

  void append_discovery_config(JsonObject* config) override;
  void handle_message(char* message) override;

  void switch_on();
  void switch_off();
  void toggle();

 protected:
  void serialize_state(char* serialized_state_buffer, size_t buffer_size) override;
  void adapt_state(char* message) override;
};

#endif