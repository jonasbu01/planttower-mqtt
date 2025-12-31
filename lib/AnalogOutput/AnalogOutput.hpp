#ifndef ANALOGOUTPUT
#define ANALOGOUTPUT

#include "Arduino.h"

class AnalogOutput {
private:
    u_int8_t pin;
    u_int8_t value = 0;
    u_int8_t current_percentage = 0;
    u_int64_t time_last_change = 0;
    bool fade_animation_step = 0;
    const uint8_t ledLUT[51] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 18, 21, 24, 27, 30, 34, 38, 42, 46, 51, 55, 60, 66, 71, 77, 83, 89, 96, 102, 109, 116, 124, 131, 139, 148, 156, 165, 174, 183, 192, 202, 212, 223, 233, 244, 255};
public:
    AnalogOutput(u_int8_t pin);
    u_int8_t get_value();
    u_int8_t get_current_percentage();
    u_int64_t get_time_last_change();
    void set_value(u_int8_t value);
    void set_level_logarithmic_percent(u_int8_t percent);
    bool fade_logarithmic_to_percent(u_int8_t target_percentage, uint16_t interval_ms = 40);
    void fade_logarithmic_between_percentages(u_int8_t percent_1, u_int8_t percent_2, uint16_t interval_1_ms = 40, uint16_t interval_2_ms = 40, uint64_t wait_time_1_ms = 0, uint64_t wait_time_2_ms = 0);
};

#endif