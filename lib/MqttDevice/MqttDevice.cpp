#include "MqttDevice.hpp"

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
  serializeJsonPretty(payload, Serial);
  serializeJson(payload, payload_buffer);

  Serial.print("Publishing discovery config to ");
  Serial.println(this->discovery_topic);

  if (client->publish(this->discovery_topic, payload_buffer, true)) {
    Serial.println("Discovery published successfully.");
  } else {
    Serial.println("Discovery publish FAILED!");
  }
}
