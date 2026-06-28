#include "Arduino.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <vector>
// Connection
#include "ConnectionManager.hpp"
// MQTT
#include "MqttDevice.hpp"
#include "MqttComponent.hpp"
#include "MqttSensor.hpp"
#include "MqttBinarySensor.hpp"
#include "MqttSwitch.hpp"
// Hardware Components
#include "HardwarePinConfig.h"
#include "DigitalInput.hpp"
#include "AnalogInput.hpp"
#include "OneWireTemperatureSensor.hpp"
#include "Pump.hpp"
#include "LedDisplay.hpp"

ConnectionManager connection_manager;
WiFiClient wifi_client;
PubSubClient* mqtt_client = nullptr;

// MQTT Components
MqttSensor* mqtt_next_pump_change_sensor = nullptr;
MqttSensor* mqtt_temperature_sensor = nullptr;
MqttBinarySensor* mqtt_waterlevel_sensor = nullptr;
MqttSwitch* mqtt_pump_switch = nullptr;
MqttSwitch* mqtt_pump_enable_switch = nullptr;
MqttDevice* mqtt_device = nullptr;

// Objects for hardware components
Pump *pump = nullptr;
DigitalInput *waterlevel_sensor = nullptr;
OneWireTemperatureSensor *temperature_sensor = nullptr;
LedDisplay *led_display = nullptr;
DigitalInput *touch_button = nullptr;
DigitalInput *reset_connectivity_button = nullptr;
DigitalInput *pump_enable_button = nullptr;

// Timing variables
uint64_t time_serial_print_interval = 0;
uint64_t time_mqtt_sensor_refresh_interval = 0;

void setup() {
  nvs.begin("esp32");
  Serial.begin(115200);
  delay(10);

  connection_manager.init();
  MqttConfig* mqtt_config = connection_manager.get_mqtt_config();

  mqtt_client = new PubSubClient(wifi_client);
  mqtt_next_pump_change_sensor = new MqttSensor(
    mqtt_client,
    (String(mqtt_config->unique_id) + "_duration_next_pump_change_s").c_str(),
    "Nächte Pumpenumschaltung",
    "duration",
    "s",
    "{{ value_json.duration }}"
  );
  mqtt_temperature_sensor = new MqttSensor(
    mqtt_client,
    (String(mqtt_config->unique_id) + "_temperature").c_str(),
    "Temperatur",
    "temperature",
    "°C",
    "{{ value_json.temperature }}"
  );
  mqtt_waterlevel_sensor = new MqttBinarySensor(
    mqtt_client,
    (String(mqtt_config->unique_id) + "_water_level_low").c_str(),
    "Wasserstand gering",
    "{{ value_json.state }}",
    new std::map<std::string, std::string>{
      {"icon", "mdi:water-alert"}
    }
  );
  mqtt_pump_switch = new MqttSwitch(
    mqtt_client,
    (String(mqtt_config->unique_id) + "_pump").c_str(),
    "Pumpe",
    new std::map<std::string, std::string>{
      {"icon", "mdi:pump"}
    }
  );
  mqtt_pump_enable_switch = new MqttSwitch(
    mqtt_client,
    (String(mqtt_config->unique_id) + "_pump_enable").c_str(),
    "Freigabe Pumpe",
    new std::map<std::string, std::string>{
      {"icon", "mdi:power"}
    },
    Bools::PumpEnabled
  );
  mqtt_device = new MqttDevice(mqtt_client, mqtt_config);

  pump = new Pump(PUMP_PIN, mqtt_pump_enable_switch, mqtt_pump_switch);
  waterlevel_sensor = new DigitalInput(WATERLEVEL_PIN, true, true);
  temperature_sensor = new OneWireTemperatureSensor(TEMPERATURE_PIN);
  led_display = new LedDisplay(GREEN_LED_PIN, RED_LED_PIN, BLUE_LED_PIN);
  touch_button = new DigitalInput(TOUCH_PIN, true, false);
  reset_connectivity_button = new DigitalInput(RESET_CONNECTIVITY_PIN, true, true);
  pump_enable_button = new DigitalInput(PUMP_ENABLE_PIN, true, true);

  mqtt_device->configure_client();
  mqtt_device->register_component(mqtt_next_pump_change_sensor)
    ->register_component(mqtt_temperature_sensor)
    ->register_component(mqtt_waterlevel_sensor)
    ->register_component(mqtt_pump_switch)
    ->register_component(mqtt_pump_enable_switch);
  mqtt_device->load_persistent_settings();
  connection_manager.set_mqtt_device(mqtt_client, mqtt_device);
  temperature_sensor->request_value();

  while(!led_display->run_startup_animation()); // wait 2-3s until animation done
  mqtt_pump_switch->switch_on(); // initial state at startup (if pump enabled)
}

void loop() {
  connection_manager.loop();

  //buttons
  //toggle pump by touch button
  touch_button->refresh_state();
  if(touch_button->rising_edge()){
    //pump->toggle(); //Hotfix - leaf in wind triggered button
  }
  //reset connectivity button
  reset_connectivity_button->refresh_state();
  if(reset_connectivity_button->get_state() && reset_connectivity_button->get_duration_ms_state_changed() > 5000){ // hold for 5s
    connection_manager.restart_in_ap_mode();
  }
  //enable/disable pump
  pump_enable_button->refresh_state();
  if(pump_enable_button->falling_edge()){
    pump->toggle_enabled();
  }

  //refresh hardware components
  temperature_sensor->request_value_by_interval(15);
  pump->run_interval_cycle(temperature_sensor, 60, 2400); //1 min = 60 s on / 40 min = 2400 s off (below 20 °C)
  if (pump->get_enabled() && !pump->get_state() && pump->get_time_switched_off() + 60000 <= millis()){ //measure only when pump is >60s off and enabled
    waterlevel_sensor->refresh_state();
  }
  led_display->display_state(pump, waterlevel_sensor, temperature_sensor, connection_manager.get_state());

  //refresh mqtt sensors (state will only be sent if value changed)
  if (time_mqtt_sensor_refresh_interval < millis() && mqtt_device->is_connected()) {
    mqtt_next_pump_change_sensor->set_state((pump->get_duration_until_change_s()/5)*5);//rounded to 5s
    mqtt_temperature_sensor->set_state(temperature_sensor->get_temperature());
    mqtt_waterlevel_sensor->set_state(waterlevel_sensor->get_state() ? MqttBinarySensor::ON_STATE : MqttBinarySensor::OFF_STATE);
    time_mqtt_sensor_refresh_interval = millis() + 500; //every 0.5s
  }
  //print states every second
  if (time_serial_print_interval < millis()){
    Serial.println("===============================");
    Serial.println("Pump enabled: " + String(pump->get_enabled() ? "enabled" : "disabled"));
    Serial.printf("Pump state: %s (in %" PRId64 " s %s)\n", pump->get_state() ? "on" : "off", pump->get_duration_until_change_s(), pump->get_state() ? "off" : "on");
    Serial.println(waterlevel_sensor->get_state()? "Water-Level: low" : "Water-Level: ok");
    Serial.println(temperature_sensor->get_error() ? "Air temp.: Error, no connection?" : String("Air temp.: ") + temperature_sensor->get_temperature() + " °C");
    Serial.println(touch_button->get_state() ? "Button: pressed" : "Button: not pressed");
    connection_manager.print_status();
    time_serial_print_interval = millis() + 1000;
  }
}