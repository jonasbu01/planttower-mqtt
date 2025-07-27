#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
#include "Secrets.hpp"
#include "MqttDevice.hpp"
#include "MqttSensor.hpp"

const char* clientId = "ESP32_Planttower";
WiFiClient espClient;
PubSubClient* client = new PubSubClient(espClient);
MqttSensor temperature_sensor(
  client,
  "aussentemperatur",
  "Außentemperatur",
  "temperature",
  "°C",
  "{{ value_json.temperature}}"
);
MqttSensor waterlevel_sensor( //neue Klasse BinarySensor
  client,
  "wasserstandgering",
  "Wasserstand gering",
  "temperature",
  "°C",
  "{{ value_json.temperature}}"
);
std::vector<MqttSensor> components = { temperature_sensor, waterlevel_sensor };
MqttDevice planttower(
  client,
  "planttower",
  "Plant Tower", //display name
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
  //mqtt
  client->setBufferSize(1024);
  client->setServer(mqtt_server, mqtt_port);
  mqtt_reconnect();
  planttower.send_discovery();
}

void loop() {
  if (!client->connected()) {
    mqtt_reconnect();
  }
  client->loop();

  temperature_sensor.send_state(21.0 + (random(0,100)/100.0)); // Sende den aktuellen Zustand des Sensors
  waterlevel_sensor.send_state(1); // Sende den aktuellen Zustand des Sensors

  delay(5000); // alle 5 Sekunden senden
}
