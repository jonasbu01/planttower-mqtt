#include "MqttSensor.hpp"

const char* MqttSensor::get_unique_id() {
  return this->unique_id;
}

void MqttSensor::append_discovery_config(JsonObject* config) {
  (*config)["platform"] = "sensor";
  (*config)["name"] = this->name;
  (*config)["unique_id"] = this->unique_id;
  (*config)["device_class"] = this->device_class;
  (*config)["state_topic"] = this->state_topic;
  (*config)["unit_of_measurement"] = this->unit;
  (*config)["value_template"] = this->value_template;;
}

void MqttSensor::send_state(float state) {
  JsonDocument payload;
  payload[this->device_class] = state;

  char state_payload[128];
  serializeJson(payload, state_payload);

  if (this->client->publish(this->state_topic, state_payload, false)) {
    Serial.print(this->name);
    Serial.print(" published state: ");
    Serial.println(state);
  } else {
    Serial.println("Publish state FAILED");
  }
}