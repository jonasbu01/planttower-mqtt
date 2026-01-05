#ifndef WIFIMANAGER
#define WIFIMANAGER

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "Secrets.hpp" //muss noch zu Persistent Settings geändert werden
#include "HTMLPages.hpp"
#include "PersistentSettings.hpp"

class WifiManager {
private:
    u64_t last_reconnect_attempt = 0;
    bool connected = false;
    bool previous_connection_status = false;
    const char* AP_SSID = "ESP32-Setup";
    const char* AP_PASS = "12345678";
    IPAddress apIP;
    IPAddress netMsk;
    DNSServer dnsServer;
    WebServer server;
    const uint16_t DNS_PORT = 53;
    void ap_handle_root();
    void ap_handle_save();
    void ap_handle_captive_portal();
public:
  WifiManager();
  void setup_wifi();
  void connection_loop();
  void ap_mode_start();
  void ap_mode_loop();
  bool is_connected();
  int get_rssi();
  void print_wifi_quality();
};

#endif