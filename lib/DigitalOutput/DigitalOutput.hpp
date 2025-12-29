#ifndef DIGITALOUTPUT
#define DIGITALOUTPUT

#include "Arduino.h"

class DigitalOutput {
private:
    u_int8_t pin;
    bool is_on = false;
    uint64_t time_switched_on = 0;
    uint64_t time_switched_off = 0;

public:
    DigitalOutput(u_int8_t pin);
    bool get_state();
    u_int64_t get_time_switched_on();
    u_int64_t get_time_switched_off();
    void switch_on();
    void switch_off();
    void toggle_switch();
};

#endif