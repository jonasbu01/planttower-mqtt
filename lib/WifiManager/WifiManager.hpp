#ifndef WIFIMANAGER
#define WIFIMANAGER

#include <WiFi.h>
#include "Secrets.hpp"

class WifiManager {
private:
    u64_t last_reconnect_attempt = 0;
    bool connected = false;
    bool previous_connection_status = false;
public:
  WifiManager();
  void setup_wifi();
  void connection_loop();
  bool get_status();
};

#endif