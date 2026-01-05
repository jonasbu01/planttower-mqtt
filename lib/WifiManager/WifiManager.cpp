#include "WifiManager.hpp"

WifiManager::WifiManager() : 
    apIP(8, 8, 8, 8), //Google DNS -> works without Internet Access
    netMsk(255, 255, 255, 0), 
    server(80)
    {}

void WifiManager::setup_wifi() {
    WiFi.setSleep(false);
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

void WifiManager::ap_handle_root() {
    this->server.send(200, "text/html", html_setup_page);
}

void WifiManager::ap_handle_captive_portal() {
    this->server.sendHeader("Location", String("http://") + this->apIP.toString(), true);
    this->server.send(302, "text/plain", "");
}

void WifiManager::ap_handle_save() {
    String wifi_ssid = server.arg("ssid");
    String wifi_password = server.arg("password");
    String mqtt_server   = server.arg("mqtt_server");
    String mqtt_port   = server.arg("mqtt_port");
    String mqtt_user   = server.arg("mqtt_user");
    String mqtt_password   = server.arg("mqtt_password");
    String mqtt_device_name   = server.arg("mqtt_device_name");
    String mqtt_device_id   = server.arg("mqtt_device_id");

    //save in persistent settings
    Serial.println("===== Save persistent Connection Settings... =====");
    //save helper variables
    bool_settings.setValue(Bools::ConTested, false);
    Serial.println("'ConnectionTested': false");
    bool_settings.setValue(Bools::ConChanged, true);
    Serial.println("'ConnectionChanged': true");
    bool_settings.setValue(Bools::APMode, false);
    Serial.println("'APMode': false");
    //Wifi
    string_settings.setValue(Strings::WIFISSID, wifi_ssid.c_str());
    Serial.printf("'Wifi SSID': %s\n", wifi_ssid.c_str());
    string_settings.setValue(Strings::WIFIPASSWORD, wifi_password.c_str());
    Serial.printf("'Wifi Password': %s\n", wifi_password.c_str());
    //MQTT-Broker
    string_settings.setValue(Strings::MQTTSERVER, mqtt_server.c_str());
    Serial.printf("'MQTT Server': %s\n", mqtt_server.c_str());
    integer_settings.setValue(Integers::MQTTPORT, mqtt_port.toInt());
    Serial.printf("'MQTT Port': %d\n", mqtt_port.toInt());
    string_settings.setValue(Strings::MQTTUSER, mqtt_user.c_str());
    Serial.printf("'MQTT User': %s\n", mqtt_user.c_str());
    string_settings.setValue(Strings::MQTTPASSWORD, mqtt_password.c_str());
    Serial.printf("'MQTT Password': %s\n", mqtt_password.c_str());
    //MQTT-Device
    string_settings.setValue(Strings::MQTTDEVICENAME, mqtt_device_name.c_str());
    Serial.printf("'MQTT Device Name': %s\n", mqtt_device_name.c_str());
    string_settings.setValue(Strings::MQTTDEVICEID, mqtt_device_id.c_str());
    Serial.printf("'MQTT Device ID': %s\n", mqtt_device_id.c_str());
    Serial.println("================ Settings saved =================");


    this->server.send(200, "text/html", html_save_page);

    ESP.restart();
}

void WifiManager::ap_mode_start() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(this->apIP, this->apIP, this->netMsk);
    WiFi.softAP(this->AP_SSID, this->AP_PASS);

    Serial.println("Access Point started");
    Serial.println(this->apIP);

    this->dnsServer.start(this->DNS_PORT, "*", this->apIP);

    this->server.on("/", [this]() {
        this->ap_handle_root();
    });
    this->server.on("/save", HTTP_POST, [this]() {
        this->ap_handle_save();
    });

    // iOS / MAC OS
    this->server.on("/hotspot-detect.html", [this]() {
        this->ap_handle_captive_portal();
    });

    this->server.on("/library/test/success.html", [this]() {
        this->ap_handle_captive_portal();
    });

    // Android
    this->server.on("/generate_204", HTTP_GET, [this]() {
        Serial.println("Captive Portal Trigger detected: /generate_204");
        this->server.send(200, "text/html", html_setup_page);
    });
    this->server.on("/gen_204", HTTP_GET, [this]() {
        Serial.println("Captive Portal Trigger detected: /gen_204");
        this->server.send(200, "text/html", html_setup_page);
    });

    // Fallback
    this->server.onNotFound([this]() {
        Serial.println("Captive Portal Fallback detected: " + this->server.uri());
        // Antwort wie bisher
        this->server.send(200, "text/html", html_setup_page);
    });
    this->server.begin();
    Serial.println("AP-Mode: HTTP-Server started");
}

void WifiManager::ap_mode_loop() {
    this->dnsServer.processNextRequest();
    this->server.handleClient();
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
        Serial.print("WiFi connected: ");
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