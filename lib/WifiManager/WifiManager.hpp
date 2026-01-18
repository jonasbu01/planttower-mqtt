#ifndef WIFIMANAGER
#define WIFIMANAGER

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "HTMLPages.hpp"
#include "PersistentSettings.hpp"

class WifiManager {
private:
    u64_t last_reconnect_attempt = 0;
    bool connected = false;
    bool previous_connection_status = false;
    const char* AP_SSID = "ESP32-Setup";
    const char* AP_PASS = "12345678";
    char wifi_ssid[64];
    char wifi_password[64];
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
  void setup_wifi(const char* ssid, const char* password);
  void connection_loop();
  void ap_mode_start();
  void ap_mode_loop();
  bool is_connected();
  int get_rssi();
  void print_wifi_quality();
};

#endif