#include "MqttDevice.hpp"

void MqttDevice::send_discovery() {
  JsonDocument payload;
  payload["state_topic"] = this->state_topic;
  payload["qos"] = 2;

  JsonObject device = payload.createNestedObject("dev");
  device["ids"] = this->unique_id;
  device["name"] = this->name;
  device["mf"] = "Jonas";
  device["mdl"] = this->name;

  JsonObject origin = payload.createNestedObject("o");
  origin["name"] = this->name;

  JsonObject components = payload.createNestedObject("cmps");

  for (auto& component : *this->components) {
    JsonObject comp = components.createNestedObject(component.get_unique_id());
    component.append_discovery_config(&comp);
  }

  char payload_buffer[1024];
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
