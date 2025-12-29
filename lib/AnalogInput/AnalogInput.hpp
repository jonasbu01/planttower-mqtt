#ifndef ANALOGINPUT
#define ANALOGINPUT

#include "Arduino.h"
#include <deque>

class AnalogInput {
private:
    u_int8_t pin;
    u_int16_t raw_value = 0;
    std::deque<u_int16_t> raw_samples;
    u_int16_t filtered_value = 0;
    const u_int16_t SAMPLE_AMOUNT_FILTER = 100; 
    const u_int16_t SAMPLE_AMOUNT_DELETE_MIN_MAX = 30; //has to be smaller than SAMPLE_AMOUNT_FILTER
    const u_int16_t ADC_BITS = 4096; 
    const float ADC_VOLTAGE_REFERENCE = 3.3;
    uint64_t time_last_sample = 0;
    void update_filtered_value();
    float calc_voltage(u_int16_t value);
public:
    AnalogInput(u_int8_t pin);
    void sample_each_ms();
    u_int16_t get_raw_value();
    float get_voltage();
    float get_filtered_voltage();
};

#endif