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
  "tempSensorTest",
  "Temperature Sensor",
  "temperature",
  "°C",
  "{{ value_json.temperature}}"
);
MqttSensor temperature_sensor2(
  client,
  "tempSensorTest2",
  "Temperature Sensor 2",
  "temperature",
  "°C",
  "{{ value_json.temperature}}"
);
std::vector<MqttSensor> components = { temperature_sensor, temperature_sensor2 };
MqttDevice planttower(
  client,
  "planttower",
  "Plant Tower",
  "Jonas",
  &components
);


void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());
}

void mqttReconnect() {
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client->connect(clientId, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Subscribe if needed here
      client->subscribe("homeassistant/status");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 1s");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupWiFi();

  client->setBufferSize(1024);
  client->setServer(mqtt_server, mqtt_port);

  mqttReconnect();
  planttower.send_discovery();
}

void loop() {
  if (!client->connected()) {
    mqttReconnect();
    // planttower.send_discovery(); // Optional: Discovery neu senden bei reconnect
  }
  client->loop();

  temperature_sensor.send_state(); // Sende den aktuellen Zustand des Sensors
  temperature_sensor2.send_state(); // Sende den aktuellen Zustand des Sensors

  delay(5000); // alle 5 Sekunden senden
}
