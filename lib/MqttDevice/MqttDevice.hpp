#ifndef MQTT_DEVICE
#define MQTT_DEVICE

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <vector>
#include "Arduino.h"
#include "MqttEntity.hpp"
#include "MqttComponent.hpp"

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
    const char* manufacturer,
    std::vector<MqttComponent*>* components
  ): MqttEntity(mqtt_client, unique_id, name, "device"),
    mqtt_credentials(mqtt_credentials),
    manufacturer(manufacturer),
    components(components) {
      snprintf(this->discovery_topic, 256, "homeassistant/%s/%s/config", this->platform, unique_id);
    }

  void connect_initially();
  void reconnect();
  boolean is_connected();
  void send_discovery();

 private:
  MqttCredentials* mqtt_credentials;
  const char* manufacturer;
  char discovery_topic[256];
  std::vector<MqttComponent*>* components;

  void configure_client();
  void connect_client();
};

#endif