#include "WifiManager.hpp"

WifiManager::WifiManager() {}

void WifiManager::setup_wifi() {
    WiFi.setSleep(false);
    this->connection_loop();
}

void WifiManager::connection_loop() {
    if (WiFi.status() != WL_CONNECTED && (millis() - this->last_reconnect_attempt) > 2000) {
        Serial.printf("WiFi disconnected, try reconnecting to %s...\n", ssid);
        delay(10);
        WiFi.begin(ssid, password);
        last_reconnect_attempt = millis();
    }
    this->previous_connection_status = this->connected;
    this->connected = WiFi.status() == WL_CONNECTED;
    if (!this->previous_connection_status && this->connected) {
        Serial.println("WiFi connected");
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
    }
}

bool WifiManager::get_status() {
    return WiFi.status() == WL_CONNECTED;   
}