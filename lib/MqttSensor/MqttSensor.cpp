#include "MqttSensor.hpp"

void MqttSensor::append_discovery_config(JsonObject* config) {
  (*config)["platform"] = "sensor";
  (*config)["name"] = this->name;
  (*config)["unique_id"] = this->unique_id;
  (*config)["device_class"] = this->device_class;
  (*config)["state_topic"] = this->state_topic;
  (*config)["unit_of_measurement"] = this->unit;
  (*config)["value_template"] = this->value_template;
}

void MqttSensor::serialize_state(char* serialized_state_buffer, size_t buffer_size) {
  JsonDocument payload;
  payload[this->device_class] = state;
  serializeJson(payload, serialized_state_buffer, buffer_size);
}