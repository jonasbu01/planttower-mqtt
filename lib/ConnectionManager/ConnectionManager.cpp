#include "ConnectionManager.hpp"

ConnectionManager::ConnectionManager() {}

void ConnectionManager::init() {
    this->load_persistent_settings();
    if (this->ap_mode) {
        this->wifi_manager.ap_mode_start();
        bool_settings.setValue(Bools::APMode, false); //for next restart
    }else{
        this->wifi_manager.setup_wifi();
    }
}

void ConnectionManager::set_mqtt_device(PubSubClient* client, MqttDevice* device) {
    this->mqtt_client = client;
    this->mqtt_device = device;
}

void ConnectionManager::load_persistent_settings() {
    Serial.println("===== Load persistent Connection Settings... =====");
    //Helper: ConnectionTested
    this->connection_tested = bool_settings.getValue(Bools::ConTested);
    Serial.printf("'ConnectionTested': %s\n", this->connection_tested ? "true" : "false");
    this->connection_changed = bool_settings.getValue(Bools::ConChanged);
    Serial.printf("'ConnectionChanged': %s\n", this->connection_changed ? "true" : "false");
    this->ap_mode = bool_settings.getValue(Bools::APMode);
    Serial.printf("'APMode': %s\n", this->ap_mode ? "true" : "false");
    //Wifi
    if (getStringSettingSafe(Strings::WIFISSID, this->wifi_ssid, sizeof(this->wifi_ssid))) {
        Serial.printf("'Wifi SSID': %s\n", this->wifi_ssid);
    }
    if (getStringSettingSafe(Strings::WIFIPASSWORD, this->wifi_password, sizeof(this->wifi_password))) {
        Serial.printf("'Wifi Password': %s\n", this->wifi_password);
    }
    //MQTT-Broker
    if (getStringSettingSafe(Strings::MQTTSERVER, this->mqtt_server, sizeof(this->mqtt_server))) {
        Serial.printf("'MQTT Server': %s\n", this->mqtt_server);
    }
    this->mqtt_port = integer_settings.getValue(Integers::MQTTPORT);
    Serial.printf("'MQTT Port': %d\n", this->mqtt_port);
    if (getStringSettingSafe(Strings::MQTTUSER, this->mqtt_user, sizeof(this->mqtt_user))) {
        Serial.printf("'MQTT User': %s\n", this->mqtt_user);
    }
    if (getStringSettingSafe(Strings::MQTTPASSWORD, this->mqtt_password, sizeof(this->mqtt_password))) {
        Serial.printf("'MQTT Password': %s\n", this->mqtt_password);
    }
    //MQTT-Device
    if (getStringSettingSafe(Strings::MQTTDEVICENAME, this->mqtt_device_name, sizeof(this->mqtt_device_name))) {
        Serial.printf("'MQTT Device Name': %s\n", this->mqtt_device_name);
    }
    if (getStringSettingSafe(Strings::MQTTDEVICEID, this->mqtt_device_id, sizeof(this->mqtt_device_id))) {
        Serial.printf("'MQTT Device ID': %s\n", this->mqtt_device_id);
    }
    Serial.println("================ Settings loaded =================");
}

void ConnectionManager::loop() {
    if (this->ap_mode) {
        //AP Mode
        this->wifi_manager.ap_mode_loop();
        if (millis() > this->TIMEOUT_AP_MODE_MS) {
            Serial.println("AP-Mode Timeout reached. Restart...");
            ESP.restart();
        }
    }else if(!this->connection_changed && !this->connection_tested){
        //WIFI disabled
        return;
    }else{
        //normal mode / testing
        //WIFI
        this->wifi_manager.connection_loop();
        //MQTT
        if (!this->mqtt_device->is_connected() && this->wifi_manager.is_connected()) { 
            this->mqtt_device->connect_client();
        }
        this->mqtt_client->loop();
        //Helper Variables
        if(!this->connection_tested && this->wifi_manager.is_connected() && this->mqtt_device->is_connected()){
            Serial.println("Connection successful with new settings.");
            bool_settings.setValue(Bools::ConTested, true);
            bool_settings.setValue(Bools::ConChanged, false);
            this->connection_tested = true;
            this->connection_changed = false;
        }
        if(this->connection_changed && (!this->wifi_manager.is_connected() || !this->mqtt_device->is_connected()) && millis() > this->TIMEOUT_TEST_CONNECTION_MS){
            Serial.println("No connection could be established with the new settings. Stopping WIFI.");
            bool_settings.setValue(Bools::ConChanged, false);
            bool_settings.setValue(Bools::ConTested, false);
            this->connection_changed = false;
            this->connection_tested = false;
        }
    }
}

void ConnectionManager::restart_in_ap_mode() {
    Serial.println("Restart in AP-Mode...");
    bool_settings.setValue(Bools::APMode, true);
    ESP.restart();
}

bool ConnectionManager::get_wifi_connected() {
    return this->wifi_manager.is_connected();
}

void ConnectionManager::print_status() {
    if (this->ap_mode) {
        //AP Mode
        Serial.println("WiFi AP-Mode activated (Timeout: " + String((this->TIMEOUT_AP_MODE_MS - (millis() % this->TIMEOUT_AP_MODE_MS)) / 1000) + " s)");
    }else if(!this->connection_changed && !this->connection_tested){
        //WIFI disabled
        Serial.println("WiFi deactivated");
        return;
    }else{
        //normal mode / testing
        Serial.println("WiFi activated");
        if(this->connection_changed && (!this->wifi_manager.is_connected() || !this->mqtt_device->is_connected()) && millis()){
            Serial.println("Connection settings changed, try to connect (Timeout: " + String((this->TIMEOUT_TEST_CONNECTION_MS - (millis() % this->TIMEOUT_TEST_CONNECTION_MS)) / 1000) + " s)");
        }
        this->wifi_manager.print_wifi_quality();
        Serial.println(mqtt_device->is_connected() ? "MQTT-Broker: connected" : "MQTT-Broker: not connected");
    }
}