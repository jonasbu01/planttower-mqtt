#include "DigitalInput.hpp"

DigitalInput::DigitalInput(u_int8_t pin, bool invert_state, bool pullup){
    this->pin = pin;
    this->invert_state = invert_state;
    if (pullup){
        pinMode(this->pin, INPUT_PULLUP);
    }else{
        pinMode(this->pin, INPUT);
    }
    
}

void DigitalInput::refresh_state(){
    this->prev_state = this->state;
    if (this->invert_state){
        this->state = !digitalRead(this->pin);
    }else{
        this->state = digitalRead(this->pin);
    }
    if (this->prev_state != this->state){
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