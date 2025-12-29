#ifndef DIGITALINPUT
#define DIGITALINPUT

#include "Arduino.h"

class DigitalInput {
private:
    u_int8_t pin;
    bool invert_state = false;
    bool prev_state = false;
    bool state = false;
    uint64_t time_state_changed = 0;
public:
    DigitalInput(u_int8_t pin, bool invert_state, bool pullup);
    void refresh_state();
    bool get_state();
    bool get_prev_state();
    bool rising_edge();
    bool falling_edge();
    uint64_t get_time_state_changed();
    uint64_t get_duration_ms_state_changed();
};

#endif