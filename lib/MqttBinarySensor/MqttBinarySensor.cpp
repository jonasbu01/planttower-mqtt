#include "MqttBinarySensor.hpp"

void MqttBinarySensor::append_discovery_config(JsonObject* config) {
  (*config)["platform"] = this->platform;;
  (*config)["name"] = this->name;
  (*config)["unique_id"] = this->unique_id;
  (*config)["state_topic"] = this->state_topic;
  (*config)["value_template"] = this->value_template;
  this->append_additional_discovery_config(config);
}

void MqttBinarySensor::serialize_state(char* serialized_state_buffer, size_t buffer_size) {
  JsonDocument payload;
  payload["state"] = this->state;
  serializeJson(payload, serialized_state_buffer, buffer_size);
}