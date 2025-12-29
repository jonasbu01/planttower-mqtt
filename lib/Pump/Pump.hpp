#ifndef PUMP
#define PUMP

#include "Arduino.h"
#include "DigitalOutput.hpp"
#include "OneWireTemperatureSensor.hpp"

class Pump : public DigitalOutput{
private:
    bool function_test_ok = true;
    int64_t duration_until_on_s = 0;
    int64_t duration_until_off_s = 0;
    uint64_t calculate_off_duration(OneWireTemperatureSensor *temperature_sensor, uint64_t off_duration_below_25C_s);
    void do_function_test();
public:
    Pump(uint8_t pump_pin);
    void run_interval_cycle(OneWireTemperatureSensor *temperature_sensor, uint64_t on_duration_s, uint64_t off_duration_below_25C_s);
    int64_t get_duration_until_on_s();
    int64_t get_duration_until_off_s();
};

#endif