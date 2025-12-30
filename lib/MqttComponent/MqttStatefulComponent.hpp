#ifndef MQTT_STATEFUL_COMPONENT
#define MQTT_STATEFUL_COMPONENT

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "MqttComponent.hpp"

template <typename T>
class MqttStatefulComponent : public MqttComponent {
 public:
  MqttStatefulComponent(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name,
    const char* platform
  ): MqttComponent(mqtt_client, unique_id, name, platform) {}

  virtual void append_discovery_config(JsonObject* config) = 0;
  void set_state(T state) {
    this->state = state;
    char state_payload[128];
    this->serialize_state(state_payload, 128);
    this->send_state(state_payload);
  };
  T get_state() {
    return this->state;
  };

 protected:
  T state;
  virtual void serialize_state(char* serialized_state_buffer, size_t buffer_size) = 0;
  void send_state(char* state_payload) {
    if (this->mqtt_client->publish(this->state_topic, state_payload, true)) {
      Serial.print(this->name);
      Serial.print(" sent state: ");
      Serial.println(state);
    } else {
      Serial.println("Sending state FAILED");
    }
  };
};

#endif