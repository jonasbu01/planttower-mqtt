#ifndef LEDDISPLAY
#define LEDDISPLAY

#include "Arduino.h"
#include "AnalogOutput.hpp"
#include "DigitalInput.hpp"
#include "Pump.hpp"
#include "OneWireTemperatureSensor.hpp"

class LedDisplay {
private:
    AnalogOutput green_led;
    AnalogOutput red_led;
    AnalogOutput blue_led;
    uint8_t startup_animation_step = 0;
    bool startup_animation_finished = false;
public:
    LedDisplay(uint8_t green_led_pin, uint8_t red_led_pin, uint8_t blue_led_pin);
    bool run_startup_animation();
    void display_state(Pump *pump, DigitalInput *waterlevel_sensor, OneWireTemperatureSensor *temperature_sensor, bool wifi_connected, bool mqtt_connected);
};

#endif