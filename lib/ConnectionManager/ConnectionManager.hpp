#ifndef CONNECTION_MANAGER
#define CONNECTION_MANAGER

#include "WifiManager.hpp"
#include <PubSubClient.h>
#include "MqttDevice.hpp"
#include "PersistentSettings.hpp"

typedef enum {
    AP_MODE,
    DEACTIVATED,
    DISCONNECTED,
    WIFI_CONNECTED,
    WIFI_MQTT_CONNECTED,
} Connectionstate;


class ConnectionManager {
private:
    WifiManager wifi_manager;
    PubSubClient* mqtt_client;
    MqttDevice* mqtt_device;
    Connectionstate state = DISCONNECTED;
    bool connection_tested = false;
    bool connection_changed = false;
    bool ap_mode = false;
    uint64_t TIMEOUT_AP_MODE_MS = 600000; //10 minutes
    uint64_t TIMEOUT_TEST_CONNECTION_MS = 120000; //2 minutes
    char wifi_ssid[64];
    char wifi_password[64];
    char mqtt_server[64];
    int mqtt_port = 1883;
    char mqtt_user[64];
    char mqtt_password[64];
    char mqtt_device_name[64];
    char mqtt_device_id[64];
    void load_persistent_settings();
    void set_state();
    
public:
  ConnectionManager();
  void init();
  void set_mqtt_device(PubSubClient* client, MqttDevice* device);
  void loop();
  void restart_in_ap_mode();
  bool get_wifi_connected();
  void print_status();
  Connectionstate get_state();
};

#endif