#include "DigitalOutput.hpp"

DigitalOutput::DigitalOutput(u_int8_t pin){
    this->pin = pin;
    pinMode(this->pin, OUTPUT);
    this->switch_off();
}

bool DigitalOutput::get_state(){
    return this->is_on;
}

u_int64_t DigitalOutput::get_time_switched_on(){
    return this->time_switched_on;
}

u_int64_t DigitalOutput::get_time_switched_off(){
    return this->time_switched_off;
}

void DigitalOutput::switch_on(){
    digitalWrite(this->pin, HIGH);
    if (!this->is_on){
        this->time_switched_on = millis();
    }
    this->is_on = true;
}

void DigitalOutput::switch_off() {
    digitalWrite(this->pin, LOW);
    if (this->is_on){
        this->time_switched_off = millis();
    }
    this->is_on = false;
}

void DigitalOutput::toggle_switch() {
    if (this->is_on){
        this->switch_off();
    }else{
        this->switch_on();
    }
}