#ifndef MQTT_SWITCH
#define MQTT_SWITCH

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "MqttControl.hpp"

class MqttSwitch : public MqttControl<const char*> {
 public:
  static constexpr const char* ON_STATE = "ON";
  static constexpr const char* OFF_STATE = "OFF";

  MqttSwitch(
    PubSubClient* mqtt_client,
    const char* unique_id,
    const char* name,
    std::map<std::string, std::string>* additional_discovery_config = new std::map<std::string, std::string>()
  ): MqttControl<const char*>(mqtt_client, unique_id, name, "switch", additional_discovery_config) {
    this->state = MqttSwitch::OFF_STATE;
  }
  bool equals_current_state(const char* other_state) override;
  void append_discovery_config(JsonObject* config) override;
  void handle_message(char* message) override;

  void on_state_change(const std::function<void(const char*)>& callback);

  void switch_on();
  void switch_off();
  void toggle();
  void set_boolean_state(bool state);
  bool is_on();

 protected:
  void serialize_state(char* serialized_state_buffer, size_t buffer_size) override;
  void adapt_state(char* message) override;

 private:
  std::function<void(const char*)> state_change_callback = [](const char*){};
};

#endif