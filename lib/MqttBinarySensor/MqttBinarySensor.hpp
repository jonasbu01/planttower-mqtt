#ifndef MQTT_BINARY_SENSOR
#define MQTT_BINARY_SENSOR

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "MqttStatefulComponent.hpp"

class MqttBinarySensor : public MqttStatefulComponent<const char*> {
 private:
  const char* value_template;

 protected:
  void serialize_state(char* serialized_state_buffer, size_t buffer_size) override;

 public:
  static constexpr const char* ON_STATE = "ON";
  static constexpr const char* OFF_STATE = "OFF";

  MqttBinarySensor(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name,
    const char* value_template
  ): MqttStatefulComponent<const char*>(mqtt_client, unique_id, name, "binary_sensor"),
    value_template(value_template) {}

  void append_discovery_config(JsonObject* config) override;
};

#endif