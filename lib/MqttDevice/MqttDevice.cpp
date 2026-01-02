#include "MqttDevice.hpp"

void MqttDevice::configure_client() {
  this->mqtt_client->setBufferSize(2048);
  this->mqtt_client->setKeepAlive(60);
  this->mqtt_client->setServer(this->mqtt_credentials->server, this->mqtt_credentials->port);
  this->mqtt_client->setCallback([this](char* topic, byte* payload, unsigned int length) {
    this->dispatch_message(topic, payload, length);
  });
}

void MqttDevice::connect_client() {
  if (!mqtt_client->connected() && (millis() - this->last_connection_attempt) > 3000) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt_client->connect(
      this->mqtt_credentials->client_id,
      this->mqtt_credentials->user,
      this->mqtt_credentials->password
    )) {
      Serial.println("connected");
      if (!this->discovery_sent){
        this->send_discovery();
      }
      this->subscribe_command_topics();
      this->publish_all_states();
    } else {
      Serial.printf("failed, rc=%d try again in 3s\n", mqtt_client->state());
    }
    this->last_connection_attempt = millis();
  }
}

void MqttDevice::subscribe_command_topics() {
  for (const auto& entry : *this->switches_registry) {
    MqttSwitch* control = entry.second;
    this->mqtt_client->subscribe(control->get_command_topic());
    Serial.printf("Subscribed to topic: %s\n", control->get_command_topic());
  }
}

boolean MqttDevice::is_connected() {
  return mqtt_client->connected();
}

MqttDevice* MqttDevice::register_component(MqttComponent* component) {
  this->components_registry->insert(
    std::make_pair(component->get_state_topic(), component)
  );
  if (MqttSwitch* control = dynamic_cast<MqttSwitch*>(component)) {
    this->switches_registry->insert(
      std::make_pair(control->get_command_topic(), control)
    );
    this->mqtt_client->subscribe(control->get_command_topic());
    Serial.printf("Subscribed to topic: %s\n", control->get_command_topic());
  }
  return this;
}

void MqttDevice::load_persistent_settings() {
  Serial.println("Loading persistent settings...");
  for (const auto& entry : *this->components_registry) {
    MqttComponent* component = entry.second;
    component->load_persistent_settings();
  }
}

void MqttDevice::send_discovery() {
  JsonDocument payload;
  payload["state_topic"] = this->state_topic;
  payload["qos"] = 2;

  JsonObject device = payload["dev"].to<JsonObject>();
  device["ids"] = this->unique_id;
  device["name"] = this->name;
  device["mf"] = "Jonas";
  device["mdl"] = this->name;

  JsonObject origin = payload["o"].to<JsonObject>();
  origin["name"] = this->name;

  JsonObject cmps = payload["cmps"].to<JsonObject>();
  for (const auto& entry : *this->components_registry) {
    MqttComponent* component = entry.second;
    JsonObject comp = cmps[component->get_unique_id()].to<JsonObject>();
    component->append_discovery_config(&comp);
  }

  char payload_buffer[2048];
  serializeJson(payload, payload_buffer);

  Serial.print("Publishing discovery config to ");
  Serial.println(this->discovery_topic);
  serializeJsonPretty(payload, Serial);
  Serial.print("\n");

  if (mqtt_client->publish(this->discovery_topic, payload_buffer, true)) {
    Serial.println("Discovery published successfully.");
    this->discovery_sent = true;
  } else {
    Serial.println("Discovery publish FAILED!");
  }
}

void MqttDevice::dispatch_message(const char* topic, byte* payload, unsigned int length) {
  Serial.printf("Received message on topic %s: ", topic);
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.println(message);
  this->switches_registry->at(topic)->handle_message(message);
}

void MqttDevice::publish_all_states() {
  Serial.println("Publishing states for all stateful components...");
  for (const auto& entry : *this->components_registry) {
    MqttComponent* component = entry.second;
    if (auto* stateful_component = dynamic_cast<MqttSwitch*>(component)) {
      stateful_component->publish_state();
    } 
    else if (auto* stateful_component = dynamic_cast<MqttBinarySensor*>(component)) {
      stateful_component->publish_state();
    } 
    else if (auto* stateful_component = dynamic_cast<MqttSensor*>(component)) {
      stateful_component->publish_state();
    }
  }
}
