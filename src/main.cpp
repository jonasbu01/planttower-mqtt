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
  "Jonas"
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
  plant_tower.register_component(&temperature_sensor)
    ->register_component(&waterlevel_sensor)
    ->register_component(&error_detected)
    ->register_component(&pump)
    ->send_discovery();
}

void loop() {
  if (!plant_tower.is_connected()) {
    plant_tower.reconnect();
  }
  mqtt_client->loop();

  static unsigned long last_update = 0;
  unsigned long now = millis();
  if (now - last_update < 60000) {
    return;
  }
  last_update = now;

  temperature_sensor.set_state(21.0 + (random(0,100)/100.0));
  waterlevel_sensor.set_state(MqttBinarySensor::ON_STATE);
  error_detected.set_state(MqttBinarySensor::OFF_STATE);
  pump.toggle();
}
