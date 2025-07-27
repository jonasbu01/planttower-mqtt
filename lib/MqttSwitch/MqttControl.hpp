#ifndef MQTT_CONTROL
#define MQTT_CONTROL

#include "MqttStatefulComponent.hpp"

template <typename T>
class MqttControl : public MqttStatefulComponent<T> {
 public:
  MqttControl(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name,
    const char* platform
  ): MqttStatefulComponent<const char*>(mqtt_client, unique_id, name, platform) {
      snprintf(this->command_topic, 256, "homeassistant/%s/%s/set", platform, unique_id);
    }

  char* get_command_topic() {
    return this->command_topic;
  };
  virtual void handle_message(char* message) = 0;

 protected:
  char command_topic[256];

  virtual void adapt_state(char* message) = 0;

  void confirm_state(char* state_payload) {
    if (this->mqtt_client->publish(this->state_topic, state_payload, true)) {
      Serial.print(this->name);
      Serial.print(" confirmed state: ");
      Serial.println(this->state);
    } else {
      Serial.println("Confirm state FAILED");
    }
  }
};

#endif