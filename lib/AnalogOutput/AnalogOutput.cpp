#include "AnalogOutput.hpp"

AnalogOutput::AnalogOutput(u_int8_t pin){
    this->pin = pin;
    pinMode(this->pin, OUTPUT);
    this->set_value(0);
}

u_int8_t AnalogOutput::get_value(){
    return this->value;
}

u_int64_t AnalogOutput::get_time_last_change(){
    return this->time_last_change;
}

void AnalogOutput::set_value(u_int8_t value){
    analogWrite(this->pin, value);
    this->time_last_change = millis();
    this->value = value;
    
}

bool AnalogOutput::fade_to_target_value(u_int8_t target_value, uint16_t interval_ms, u_int8_t step_size){
    if (this->value < target_value){
        return this->increase_to_target_value(target_value, interval_ms, step_size);
    }else{
        return this->decrease_to_target_value(target_value, interval_ms, step_size);
    }
}

bool AnalogOutput::increase_to_target_value(u_int8_t target_value, uint16_t interval_ms, u_int8_t step_size){
    if (this->time_last_change + interval_ms <= millis()){
        int16_t new_value = this->value + step_size;
        if (new_value > target_value){
            new_value = target_value;
        }
        if (new_value > 255){
            new_value = 255;
        }
        this->set_value(new_value);
    }
    return target_value == this->value;
}

bool AnalogOutput::decrease_to_target_value(u_int8_t target_value, uint16_t interval_ms, u_int8_t step_size){
    if (this->time_last_change + interval_ms <= millis()){
        int16_t new_value = this->value - step_size;
        if (new_value < target_value){
            new_value = target_value;
        }
        if (new_value < 0){
            new_value = 0;
        }
        this->set_value(new_value);
    }
    return target_value == this->value;
}

void AnalogOutput::fade_between_two_values(u_int8_t value_1, u_int8_t value_2, uint16_t interval_1_ms, uint16_t interval_2_ms, u_int8_t step_size_1, u_int8_t step_size_2, uint64_t wait_time_1_ms, uint64_t wait_time_2_ms){
    if (!this->fade_animation_step){
        if (this->value != value_1){ //fade to value 1
            this->fade_to_target_value(value_1, interval_1_ms, step_size_1);
        }else{
            if (this->time_last_change + wait_time_1_ms <= millis()){
                this->fade_animation_step = true;
            }
        }
    }else{
        if (this->value != value_2){ //fade to value 2
            this->fade_to_target_value(value_2, interval_2_ms, step_size_2);
        }else{
            if (this->time_last_change + wait_time_2_ms <= millis()){
                this->fade_animation_step = false;
            }
        }
    }
}