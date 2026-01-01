#include "WifiManager.hpp"

WifiManager::WifiManager() {}

void WifiManager::setup_wifi() {
    WiFi.setSleep(false);
    this->connection_loop();
}

void WifiManager::connection_loop() {
    if (WiFi.status() != WL_CONNECTED && (millis() - this->last_reconnect_attempt) > 3000) {
        Serial.printf("WiFi disconnected, try connecting to %s...\n", ssid);
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

bool WifiManager::is_connected() {
    return WiFi.status() == WL_CONNECTED;   
}

int WifiManager::get_rssi() {
    if (this->is_connected()) {
        return WiFi.RSSI();
    } else {
        return 0;
    }
}

void WifiManager::print_wifi_quality() {
    if (this->is_connected()) {
        int rssi = this->get_rssi();
        Serial.print("WiFi: ");
        Serial.print(rssi);
        Serial.print(" dBm ");
        if (rssi >= -50) {
            Serial.println("(Quality: Excellent)");
        } else if (rssi >= -60) {
            Serial.println("(Quality: Good)");
        } else if (rssi >= -70) {
            Serial.println("(Quality: Okay)");
        } else if (rssi >= -80) {
            Serial.println("(Quality: Bad)");
        } else {
            Serial.println("(Quality: Very Bad)");
        }
    } else {
        Serial.println("WiFi: no connection");
    }
}