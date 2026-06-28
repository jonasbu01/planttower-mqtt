#include "DigitalInput.hpp"

bool DigitalInput::read_input() {
    if (this->invert_state) {
        return !digitalRead(this->pin);
    }
    return digitalRead(this->pin);
}

DigitalInput::DigitalInput(u_int8_t pin, bool invert_state, bool pullup, uint32_t debounce_ms){
    this->pin = pin;
    this->invert_state = invert_state;
    this->debounce_ms = debounce_ms;
    if (pullup){
        pinMode(this->pin, INPUT_PULLUP);
    }else{
        pinMode(this->pin, INPUT);
    }

    this->raw_state = this->read_input();
    this->state = this->raw_state;
    this->prev_state = this->state;
    this->last_raw_change_time = millis();
}

void DigitalInput::refresh_state(){
    bool new_raw_state = this->read_input();
    if (new_raw_state != this->raw_state) {
        this->raw_state = new_raw_state;
        this->last_raw_change_time = millis();
    }

    this->prev_state = this->state;
    if (this->raw_state != this->state && (millis() - this->last_raw_change_time) >= this->debounce_ms) {
        this->state = this->raw_state;
    }

    if (this->prev_state != this->state) {
        this->time_state_changed = millis();
    }
}

bool DigitalInput::get_state(){
    return this->state;
}

bool DigitalInput::get_prev_state(){
    return this->prev_state;
}

bool DigitalInput::rising_edge(){
    return this->state && !this->prev_state;
}

bool DigitalInput::falling_edge(){
    return !this->state && this->prev_state;
}

uint64_t DigitalInput::get_time_state_changed(){
    return this->time_state_changed;
}

uint64_t DigitalInput::get_duration_ms_state_changed(){
    this->refresh_state();
    return millis() - this->time_state_changed;
}