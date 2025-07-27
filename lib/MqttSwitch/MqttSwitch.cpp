#include "MqttSwitch.hpp"

void MqttSwitch::append_discovery_config(JsonObject* config) {
  (*config)["name"] = this->name;
  (*config)["unique_id"] = this->unique_id;
  (*config)["platform"] = this->platform;
  (*config)["state_topic"] = this->state_topic;
  (*config)["command_topic"] = this->command_topic;
}

void MqttSwitch::switch_on() {
  this->set_state(MqttSwitch::ON_STATE);
}

void MqttSwitch::switch_off() {
  this->set_state(MqttSwitch::OFF_STATE);
}

void MqttSwitch::toggle() {
  if (this->state == MqttSwitch::ON_STATE) {
    this->switch_off();
  } else {
    this->switch_on();
  }
}

void MqttSwitch::serialize_state(char* serialized_state_buffer, size_t buffer_size) {
  strncpy(serialized_state_buffer, this->state, buffer_size);
}