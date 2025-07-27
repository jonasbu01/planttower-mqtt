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

const char* clientId = "ESP32_Planttower";

WiFiClient espClient;
PubSubClient* client = new PubSubClient(espClient);

MqttSensor temperature_sensor(
  client,
  "outside_temperature",
  "Außentemperatur",
  "temperature",
  "°C",
  "{{ value_json.temperature }}"
);

MqttBinarySensor waterlevel_sensor(
  client,
  "water_level_low",
  "Wasserstand gering",
  "{{ value_json.state }}"
);

MqttBinarySensor error_detected(
  client,
  "error_detected",
  "Fehler erkannt",
  "{{ value_json.state }}"
);

MqttSwitch pump(
  client,
  "pump",
  "Pumpe"
);

std::vector<MqttComponent*> components = {
  &temperature_sensor,
  &waterlevel_sensor,
  &error_detected,
  &pump,
};

MqttDevice planttower(
  client,
  "planttower",
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

void mqtt_reconnect() {
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client->connect(clientId, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Subscribe if needed here
      client->subscribe("homeassistant/status");
    } else {
      Serial.printf("failed, rc=%d try again in 1s\n", client->state());
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  client->setBufferSize(2048);
  client->setServer(mqtt_server, mqtt_port);
  mqtt_reconnect();

  planttower.send_discovery();
}

void loop() {
  if (!client->connected()) {
    mqtt_reconnect();
  }
  client->loop();

  temperature_sensor.set_state(21.0 + (random(0,100)/100.0));
  waterlevel_sensor.set_state(MqttBinarySensor::ON_STATE);
  error_detected.set_state(MqttBinarySensor::OFF_STATE);
  pump.switch_on();

  delay(5 * 1000);
}
