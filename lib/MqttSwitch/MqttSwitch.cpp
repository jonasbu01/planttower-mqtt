#include "MqttSwitch.hpp"

void MqttSwitch::append_discovery_config(JsonObject* config) {
  (*config)["name"] = this->MqttStatefulComponent::name;
  (*config)["unique_id"] = this->MqttStatefulComponent::unique_id;
  (*config)["platform"] = this->MqttStatefulComponent::platform;
  (*config)["state_topic"] = this->state_topic;
  (*config)["command_topic"] = this->command_topic;
}

void MqttSwitch::handle_message(char* message) {
  Serial.println("switch message received");
  if (strcmp(message, MqttSwitch::ON_STATE) == 0 || strcmp(message, MqttSwitch::OFF_STATE) == 0) {
    this->adapt_state(message);
    Serial.print(this->name);
    Serial.print(" turned ");
    Serial.print(this->state);
    Serial.println(" via homeassistant");
  } else {
    Serial.print(this->name);
    Serial.print(" received invalid message: ");
    Serial.println(message);
  }
}

void MqttSwitch::adapt_state(char* message) {
  if (strcmp(message, MqttSwitch::ON_STATE) == 0) {
    this->state = MqttSwitch::ON_STATE;
  } else if (strcmp(message, MqttSwitch::OFF_STATE) == 0) {
    this->state = MqttSwitch::OFF_STATE;
  }
  char state_payload[128];
  this->serialize_state(state_payload, 128);
  this->confirm_state(state_payload);
  this->state_change_callback(this->state);
}

void MqttSwitch::on_state_change(const std::function<void(const char*)>& callback) {
  this->state_change_callback = callback;
}

void MqttSwitch::switch_on() {
  this->set_state(MqttSwitch::ON_STATE);
  this->state_change_callback(this->state);
}

void MqttSwitch::switch_off() {
  this->set_state(MqttSwitch::OFF_STATE);
  this->state_change_callback(this->state);
}

void MqttSwitch::toggle() {
  if (strcmp(this->state, MqttSwitch::ON_STATE) == 0) {
    this->switch_off();
    this->state_change_callback(this->state);
  } else if (strcmp(this->state, MqttSwitch::OFF_STATE) == 0) {
    this->switch_on();
    this->state_change_callback(this->state);
  } else {
    Serial.print("Invalid state for toggle: ");
    Serial.println(this->state);
  }
}

bool MqttSwitch::is_on() {
  return strcmp(this->state, MqttSwitch::ON_STATE) == 0;
}

void MqttSwitch::serialize_state(char* serialized_state_buffer, size_t buffer_size) {
  strncpy(serialized_state_buffer, this->state, buffer_size);
}