#include "Arduino.h"
#include "WifiManager.hpp"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
//MQTT Components
#include "MqttDevice.hpp"
#include "MqttComponent.hpp"
#include "MqttSensor.hpp"
#include "MqttBinarySensor.hpp"
#include "MqttSwitch.hpp"
//Hardware Components
#include "HardwarePinConfig.h"
#include "DigitalInput.hpp"
#include "AnalogInput.hpp"
#include "OneWireTemperatureSensor.hpp"
#include "Pump.hpp"
#include "LedDisplay.hpp"

WifiManager wifi_manager;
WiFiClient wifi_client;
PubSubClient* mqtt_client = new PubSubClient(wifi_client);

//Objects
DigitalInput *waterlevel_sensor = new DigitalInput(WATERLEVEL_PIN, true, true);
OneWireTemperatureSensor *temperature_sensor = new OneWireTemperatureSensor(TEMPERATURE_PIN);
DigitalInput *touch_button = new DigitalInput(TOUCH_PIN, true, false);
LedDisplay *led_display = new LedDisplay(GREEN_LED_PIN, RED_LED_PIN, BLUE_LED_PIN);



//Variables
uint64_t time_serial_print_interval = 0;

MqttSensor* mqtt_temperature_sensor = new MqttSensor(
  mqtt_client,
  "outside_temperature",
  "Außentemperatur",
  "temperature",
  "°C",
  "{{ value_json.temperature }}"
);

MqttBinarySensor* mqtt_waterlevel_sensor = new MqttBinarySensor(
  mqtt_client,
  "water_level_low",
  "Wasserstand gering",
  "{{ value_json.state }}"
);

MqttSwitch* mqtt_test_switch = new MqttSwitch(
  mqtt_client,
  "schalter_test",
  "Schalter Test"
);

MqttSwitch* mqtt_pump_switch = new MqttSwitch(
  mqtt_client,
  "pump",
  "Pumpe"
);

Pump *mqtt_pump = new Pump(PUMP_PIN, mqtt_pump_switch);

MqttCredentials mqtt_credentials = {
  mqtt_server,
  mqtt_port,
  "mqtt_device",
  mqtt_user,
  mqtt_pass
};

MqttDevice mqtt_device(
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
  if (!mqtt_device.is_connected() && wifi_manager.is_connected()) { 
    mqtt_device.connect_client();
  }
  mqtt_client->loop();
}

void setup() {
  Serial.begin(115200);
  delay(10);
  wifi_manager.setup_wifi();
  mqtt_device.configure_client();
  mqtt_device.register_component(mqtt_temperature_sensor)
    ->register_component(mqtt_waterlevel_sensor)
    ->register_component(mqtt_pump_switch)
    ->register_component(mqtt_test_switch);
  temperature_sensor->request_value();
  mqtt_pump->switch_on(); //initial phase after start up
}

void loop() {
  handle_connections();

  if (led_display->run_startup_animation()){
    touch_button->refresh_state();
    if (!mqtt_pump->get_state() && mqtt_pump->get_time_switched_off() + 5000 <= millis()){ //measure only when pump is off
      waterlevel_sensor->refresh_state();
    }
    temperature_sensor->request_value_by_interval(15);
    mqtt_pump->run_interval_cycle(temperature_sensor, 60, 60); //1 min = 60 s on / 40 min = 2400 s off (below 20 °C)
    led_display->display_state(mqtt_pump, waterlevel_sensor, temperature_sensor);

    if (time_serial_print_interval < millis()){ //every 10 seconds
      //print states
      Serial.println("=====================");
      Serial.print("Pumpe: ");
      if (mqtt_pump->get_state()){
        Serial.println("an (in " + String(mqtt_pump->get_duration_until_off_s()) + " s aus)");
      }else{
        Serial.println("aus (in " + String(mqtt_pump->get_duration_until_on_s()) + " s an)");
      }
      if (waterlevel_sensor->get_state()){
        Serial.println("Wasserlevel: niedrig");
      }else{
        Serial.println("Wasserlevel: ok");
      }
      if (temperature_sensor->get_error()){
        Serial.println("Lufttemp.: Fehler, keine Verbindung?");
      }else{
        Serial.println("Lufttemp.: " + String(temperature_sensor->get_temperature()) + " °C");
      }
      if (touch_button->get_state()){
        Serial.println("Taste: betätigt");
      }else{
        Serial.println("Taste: nicht betätigt");
      }

      mqtt_test_switch->switch_on(); //test publish      
      //to do: only publish when changed + initial sent after mqtt connection
      mqtt_temperature_sensor->set_state(temperature_sensor->get_temperature());
      mqtt_waterlevel_sensor->set_state(waterlevel_sensor->get_state() ? MqttBinarySensor::ON_STATE : MqttBinarySensor::OFF_STATE);


      time_serial_print_interval = millis() + 10000;
    }
  }
}