#include "MqttSwitch.hpp"

bool MqttSwitch::equals_current_state(const char* other_state) {
  return strcmp(this->state, other_state) == 0;
}

void MqttSwitch::load_persistent_settings() {
  if (this->persistent_settings_key != Bools::None) {
    if (bool_settings.getValue(this->persistent_settings_key)) {
      this->switch_on();
      Serial.print("Loaded state ON for ");
      Serial.println(this->unique_id);
    } else {
      this->switch_off();
      Serial.print("Loaded state OFF for ");
      Serial.println(this->unique_id);
    }
  }
}

void MqttSwitch::append_discovery_config(JsonObject* config) {
  (*config)["name"] = this->MqttStatefulComponent::name;
  (*config)["unique_id"] = this->MqttStatefulComponent::unique_id;
  (*config)["platform"] = this->MqttStatefulComponent::platform;
  (*config)["state_topic"] = this->state_topic;
  (*config)["command_topic"] = this->command_topic;
  this->append_additional_discovery_config(config);
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

void MqttSwitch::on_state_change(const std::function<void(const char*)>& callback) {
  this->state_change_callback = callback;
}

void MqttSwitch::adapt_state(char* message) {
  if (strcmp(message, MqttSwitch::ON_STATE) == 0) {
    this->switch_on();
  } else if (strcmp(message, MqttSwitch::OFF_STATE) == 0) {
    this->switch_off();
  }
}

void MqttSwitch::toggle() {
  if (strcmp(this->state, MqttSwitch::ON_STATE) == 0) {
    this->switch_off();
  } else if (strcmp(this->state, MqttSwitch::OFF_STATE) == 0) {
    this->switch_on();
  } else {
    Serial.print("Invalid state for toggle: ");
    Serial.println(this->state);
  }
}

void MqttSwitch::switch_on() {
  if (this->persistent_settings_key != Bools::None) {
    bool_settings.setValue(this->persistent_settings_key, true);
  }
  this->set_state(MqttSwitch::ON_STATE);
  this->state_change_callback(this->state);
}

void MqttSwitch::switch_off() {
  if (this->persistent_settings_key != Bools::None) {
    bool_settings.setValue(this->persistent_settings_key, false);
  }
  this->set_state(MqttSwitch::OFF_STATE);
  this->state_change_callback(this->state);
}

void MqttSwitch::set_boolean_state(bool state) {
  if (state) {
    this->switch_on();
  } else {
    this->switch_off();
  }
}

bool MqttSwitch::is_on() {
  return strcmp(this->state, MqttSwitch::ON_STATE) == 0;
}

void MqttSwitch::serialize_state(char* serialized_state_buffer, size_t buffer_size) {
  strncpy(serialized_state_buffer, this->state, buffer_size);
}