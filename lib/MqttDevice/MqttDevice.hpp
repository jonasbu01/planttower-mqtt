#ifndef MQTT_DEVICE
#define MQTT_DEVICE

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <map>
#include "Arduino.h"
#include "MqttEntity.hpp"
#include "MqttComponent.hpp"
#include "MqttSwitch.hpp"
#include "MqttSensor.hpp"
#include "MqttBinarySensor.hpp"

struct MqttConfig {
  const char* unique_id;
  const char* name;
  const char* manufacturer;
  const char* server;
  const int port;
  const char* client_id;
  const char* user;
  const char* password;

  MqttConfig(const char* unique_id, const char* name, const char* manufacturer, const char* server, const int port, const char* client_id, const char* user, const char* password):
    unique_id(unique_id), name(name), manufacturer(manufacturer), server(server), port(port), client_id(client_id), user(user), password(password) {}
};

class MqttDevice : public MqttEntity {
 public:
  MqttDevice(
    PubSubClient* mqtt_client,
    MqttConfig* mqtt_config
  ): MqttEntity(mqtt_client, mqtt_config->unique_id, mqtt_config->name, "device"),
    manufacturer(mqtt_config->manufacturer),
    mqtt_config(mqtt_config) {
      snprintf(this->discovery_topic, 256, "homeassistant/%s/%s/config", this->platform, unique_id);
    }

  void configure_client();
  void connect_client();
  boolean is_connected();

  MqttDevice* register_component(MqttComponent* component);
  void load_persistent_settings();
  void subscribe_command_topics();
  void send_discovery();
  void publish_all_states();

  ~MqttDevice() {
    delete this->components_registry;
    delete this->switches_registry;
  }

 private:
  MqttConfig* mqtt_config;
  const char* manufacturer;
  char discovery_topic[256];
  std::map<std::string, MqttComponent*>* components_registry = new std::map<std::string, MqttComponent*>();
  std::map<std::string, MqttSwitch*>* switches_registry = new std::map<std::string, MqttSwitch*>();
  bool discovery_sent = false;
  uint64_t last_connection_attempt = 0;
  void dispatch_message(const char* topic, byte* payload, unsigned int length);
};

#endif