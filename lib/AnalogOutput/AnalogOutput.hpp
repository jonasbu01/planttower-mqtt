#ifndef ANALOGOUTPUT
#define ANALOGOUTPUT

#include "Arduino.h"

class AnalogOutput {
private:
    u_int8_t pin;
    u_int8_t value = 0;
    u_int64_t time_last_change = 0;
    bool fade_animation_step = 0;

public:
    AnalogOutput(u_int8_t pin);
    u_int8_t get_value();
    u_int64_t get_time_last_change();
    void set_value(u_int8_t value);
    bool fade_to_target_value(u_int8_t target_value, uint16_t interval_ms = 10, u_int8_t step_size = 1);
    bool increase_to_target_value(u_int8_t target_value, uint16_t interval_ms = 10, u_int8_t step_size = 1);
    bool decrease_to_target_value(u_int8_t target_value, uint16_t interval_ms = 10, u_int8_t step_size = 1);
    void fade_between_two_values(u_int8_t value_1, u_int8_t value_2, uint16_t interval_1_ms = 10, uint16_t interval_2_ms = 10, u_int8_t step_size_1 = 1, u_int8_t step_size_2 = 1, uint64_t wait_time_1_ms = 0, uint64_t wait_time_2_ms = 0);
};

#endif