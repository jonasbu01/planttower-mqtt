#ifndef CONNECTION_MANAGER
#define CONNECTION_MANAGER

#include "WifiManager.hpp"
#include <PubSubClient.h>
#include "MqttDevice.hpp"
#include "PersistentSettings.hpp"

class ConnectionManager {
private:
    WifiManager wifi_manager;
    PubSubClient* mqtt_client;
    MqttDevice* mqtt_device;
    bool connection_tested = false;
    char wifi_ssid[64];
    char wifi_password[64];
    char mqtt_server[64];
    int mqtt_port = 1883;
    char mqtt_user[64];
    char mqtt_password[64];
    char mqtt_device_name[64];
    char mqtt_device_id[64];
    void load_persistent_settings();
    
public:
  ConnectionManager();
  void init();
  void set_mqtt_device(PubSubClient* client, MqttDevice* device);
  void loop();
  bool get_wifi_connected();
  void print_status();
};

#endif