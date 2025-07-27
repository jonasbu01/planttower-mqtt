#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
#include "Secrets.hpp"
#include "MqttDevice.hpp"
#include "MqttComponent.hpp"
#include "MqttSensor.hpp"
#include "MqttBinarySensor.hpp"
#include "MqttSwitch.hpp"

WiFiClient wifi_client;
PubSubClient* mqtt_client = new PubSubClient(wifi_client);

MqttSensor temperature_sensor(
  mqtt_client,
  "outside_temperature",
  "Außentemperatur",
  "temperature",
  "°C",
  "{{ value_json.temperature }}"
);

MqttBinarySensor waterlevel_sensor(
  mqtt_client,
  "water_level_low",
  "Wasserstand gering",
  "{{ value_json.state }}"
);

MqttBinarySensor error_detected(
  mqtt_client,
  "error_detected",
  "Fehler erkannt",
  "{{ value_json.state }}"
);

MqttSwitch pump(
  mqtt_client,
  "pump",
  "Pumpe"
);

std::vector<MqttComponent*> components = {
  &temperature_sensor,
  &waterlevel_sensor,
  &error_detected,
  &pump,
};

MqttCredentials mqtt_credentials = {
  mqtt_server,
  mqtt_port,
  "MQTT_Plant_Tower",
  mqtt_user,
  mqtt_password
};

MqttDevice plant_tower(
  mqtt_client,
  &mqtt_credentials,
  "plant_tower",
  "Plant Tower",
  "Jonas",
  &components
);

void setup_wifi() {
  delay(10);
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  plant_tower.connect_initially();
  plant_tower.send_discovery();
}

void loop() {
  if (!plant_tower.is_connected()) {
    plant_tower.reconnect();
  }
  mqtt_client->loop();

  temperature_sensor.set_state(21.0 + (random(0,100)/100.0));
  waterlevel_sensor.set_state(MqttBinarySensor::ON_STATE);
  error_detected.set_state(MqttBinarySensor::OFF_STATE);
  pump.switch_on();

  delay(5 * 1000);
}
