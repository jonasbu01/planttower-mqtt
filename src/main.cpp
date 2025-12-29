#include "Arduino.h"
#include "WifiManager.hpp"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
#include "MqttDevice.hpp"
#include "MqttComponent.hpp"
#include "MqttSensor.hpp"
#include "MqttBinarySensor.hpp"
#include "MqttSwitch.hpp"

WifiManager wifi_manager;
WiFiClient wifi_client;
PubSubClient* mqtt_client = new PubSubClient(wifi_client);

bool state_bool_test = false;

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
  mqtt_pass
};

MqttDevice plant_tower_mqtt(
  mqtt_client,
  &mqtt_credentials,
  "plant_tower",
  "Plant Tower",
  "Jonas"
);

void handle_connections(){
  //Wifi
  wifi_manager.connection_loop();
  //MQTT
  if (!plant_tower_mqtt.is_connected() && wifi_manager.is_connected()) { 
    plant_tower_mqtt.connect_client();
  }
  mqtt_client->loop();
}

void setup() {
  Serial.begin(115200);
  delay(10);
  wifi_manager.setup_wifi();
  plant_tower_mqtt.configure_client();
  plant_tower_mqtt.register_component(&temperature_sensor)
  ->register_component(&waterlevel_sensor)
  ->register_component(&error_detected)
  ->register_component(&pump);
}

void loop() {
  handle_connections();


  
  static unsigned long last_update = 0;
  unsigned long now = millis();
  if (now - last_update < 10000) {
    return;
  }
  last_update = now;
  Serial.println("loop");
  temperature_sensor.set_state(21.0 + (random(0,100)/100.0));
  state_bool_test = !state_bool_test;
  if (state_bool_test) {
    waterlevel_sensor.set_state(MqttBinarySensor::ON_STATE);
    error_detected.set_state(MqttBinarySensor::OFF_STATE);
  } else {
    waterlevel_sensor.set_state(MqttBinarySensor::OFF_STATE);
    error_detected.set_state(MqttBinarySensor::ON_STATE);
  }
  pump.toggle();
}
