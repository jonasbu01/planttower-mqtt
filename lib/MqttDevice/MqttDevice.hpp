#ifndef MQTT_DEVICE
#define MQTT_DEVICE

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <map>
#include "Arduino.h"
#include "MqttEntity.hpp"
#include "MqttComponent.hpp"
#include "MqttSwitch.hpp"

struct MqttCredentials {
  const char* server;
  const int port;
  const char* client_id;
  const char* user;
  const char* password;
};

class MqttDevice : public MqttEntity {
 public:
  MqttDevice(
    PubSubClient* mqtt_client,
    MqttCredentials* mqtt_credentials,
    const char* unique_id,
    const char* name,
    const char* manufacturer
  ): MqttEntity(mqtt_client, unique_id, name, "device"),
    mqtt_credentials(mqtt_credentials),
    manufacturer(manufacturer) {
      snprintf(this->discovery_topic, 256, "homeassistant/%s/%s/config", this->platform, unique_id);
    }

  void connect_initially();
  void reconnect();
  boolean is_connected();

  MqttDevice* register_component(MqttComponent* component);
  const MqttDevice* register_component(MqttComponent* component) const;
  void send_discovery();

  ~MqttDevice() {
    delete this->components_registry;
    delete this->switches_registry;
  }

 private:
  MqttCredentials* mqtt_credentials;
  const char* manufacturer;
  char discovery_topic[256];
  std::map<std::string, MqttComponent*>* components_registry = new std::map<std::string, MqttComponent*>();
  std::map<std::string, MqttSwitch*>* switches_registry = new std::map<std::string, MqttSwitch*>();

  void configure_client();
  void connect_client();
  void dispatch_message(const char* topic, byte* payload, unsigned int length);
};

#endif