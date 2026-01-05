#include "ConnectionManager.hpp"

ConnectionManager::ConnectionManager() {}

void ConnectionManager::init() {
    this->wifi_manager.setup_wifi();
    this->load_persistent_settings();
}

void ConnectionManager::set_mqtt_device(PubSubClient* client, MqttDevice* device) {
    this->mqtt_client = client;
    this->mqtt_device = device;
}

void ConnectionManager::load_persistent_settings() {
    Serial.println("===== Lade persistente Verbindungseinstellungen... =====");
    //Helper: ConnectionTested
    this->connection_tested = bool_settings.getValue(Bools::ConnectionTested);
    Serial.printf("'ConnectionTested': %s\n", this->connection_tested ? "true" : "false");
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
    Serial.println("================ Einstellungen geladen =================");
}

void ConnectionManager::loop() {
    //Wenn connection tested, dann normaler Betrieb
    
    //wenn connection nicht getestet, dann Verbindung aufbauen und testen -> 60s Timeout, dann Aus
    
    //wenn Reset -> AP Mode (mit Timeout 10 min?)
    
    //WIFI
    wifi_manager.connection_loop();
    //MQTT
    if (!this->mqtt_device->is_connected() && wifi_manager.is_connected()) { 
        this->mqtt_device->connect_client();
    }
    this->mqtt_client->loop();
}

bool ConnectionManager::get_wifi_connected() {
    return this->wifi_manager.is_connected();
}

void ConnectionManager::print_status() {
    this->wifi_manager.print_wifi_quality();
    Serial.println(mqtt_device->is_connected() ? "MQTT-Broker: connected" : "MQTT-Broker: not connected");
}