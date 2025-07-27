#include "MqttDevice.hpp"

void MqttDevice::connect_initially() {
  this->configure_client();
  this->connect_client();
}

void MqttDevice::reconnect() {
  this->connect_client();
}

void MqttDevice::configure_client() {
  this->mqtt_client->setBufferSize(2048);
  this->mqtt_client->setServer(this->mqtt_credentials->server, this->mqtt_credentials->port);
  this->mqtt_client->setCallback([this](char* topic, byte* payload, unsigned int length) {
    this->dispatch_message(topic, payload, length);
  });
}

void MqttDevice::connect_client() {
  while (!mqtt_client->connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt_client->connect(
      this->mqtt_credentials->client_id,
      this->mqtt_credentials->user,
      this->mqtt_credentials->password
    )) {
      Serial.println("connected");
    } else {
      Serial.printf("failed, rc=%d try again in 1s\n", mqtt_client->state());
      delay(1000);
    }
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
  }
  return this;
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
