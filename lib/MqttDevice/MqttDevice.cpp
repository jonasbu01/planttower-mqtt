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

  JsonObject components = payload["cmps"].to<JsonObject>();
  for (auto& component : *this->components) {
    JsonObject comp = components[component->get_unique_id()].to<JsonObject>();
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
