#ifndef MQTT_SENSOR
#define MQTT_SENSOR

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "MqttStatefulComponent.hpp"

class MqttSensor : public MqttStatefulComponent<float> {
 private:
  const char* device_class;
  const char* unit;
  const char* value_template;

 protected:
  void serialize_state(char* serialized_state_buffer, size_t buffer_size) override;

 public:
  MqttSensor(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name,
    const char* device_class,
    const char* unit,
    const char* value_template
  ): MqttStatefulComponent<float>(mqtt_client, unique_id, name, "sensor"),
    device_class(device_class),
    unit(unit),
    value_template(value_template) {}

  void append_discovery_config(JsonObject* config) override;
};

#endif