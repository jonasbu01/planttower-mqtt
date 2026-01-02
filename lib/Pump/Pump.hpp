#ifndef PUMP
#define PUMP

#include "Arduino.h"
#include "DigitalOutput.hpp"
#include "MqttSwitch.hpp"
#include "OneWireTemperatureSensor.hpp"

class Pump : public DigitalOutput {
private:
    bool enabled = true;
    bool previous_enabled = true;
    bool function_test_ok = true;
    int64_t duration_until_on_s = 0;
    int64_t duration_until_off_s = 0;
    MqttSwitch* mqtt_pump_enable_switch;
    MqttSwitch* mqtt_pump_switch;
    uint64_t calculate_off_duration(OneWireTemperatureSensor *temperature_sensor, uint64_t off_duration_below_25C_s);
public:
    Pump(uint8_t pump_pin, MqttSwitch* mqtt_pump_enable_switch, MqttSwitch* mqtt_pump_switch);
    void run_interval_cycle(OneWireTemperatureSensor *temperature_sensor, uint64_t on_duration_s, uint64_t off_duration_below_25C_s);
    int64_t get_duration_until_on_s();
    int64_t get_duration_until_off_s();
    int64_t get_duration_until_change_s();
    bool get_enabled();
};

#endif