#ifndef DIGITALINPUT
#define DIGITALINPUT

#include "Arduino.h"

class DigitalInput {
private:
    u_int8_t pin;
    bool invert_state = false;
    bool prev_state = false;
    bool state = false;
    bool raw_state = false;
    uint32_t debounce_ms = 20;
    uint64_t time_state_changed = 0;
    uint64_t last_raw_change_time = 0;

    bool read_input();
public:
    DigitalInput(u_int8_t pin, bool invert_state, bool pullup, uint32_t debounce_ms = 20);
    void refresh_state();
    bool get_state();
    bool get_prev_state();
    bool rising_edge();
    bool falling_edge();
    uint64_t get_time_state_changed();
    uint64_t get_duration_ms_state_changed();
};

#endif