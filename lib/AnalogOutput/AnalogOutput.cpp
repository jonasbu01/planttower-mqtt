#include "AnalogOutput.hpp"

AnalogOutput::AnalogOutput(u_int8_t pin){
    this->pin = pin;
    pinMode(this->pin, OUTPUT);
    this->set_value(0);
}

u_int8_t AnalogOutput::get_value(){
    return this->value;
}

u_int8_t AnalogOutput::get_current_percentage(){
    return this->current_percentage;
}

u_int64_t AnalogOutput::get_time_last_change(){
    return this->time_last_change;
}

void AnalogOutput::set_value(u_int8_t value){
    analogWrite(this->pin, value);
    this->time_last_change = millis();
    this->value = value;
    
}

void AnalogOutput::set_level_logarithmic_percent(u_int8_t percent){
    if (percent > 100){
        percent = 100;
    }
    this->current_percentage = percent;
    u_int8_t index = percent / 2; //50 steps in LUT
    uint8_t value = this->ledLUT[index];   
    this->set_value(value);
}

bool AnalogOutput::fade_logarithmic_to_percent(u_int8_t target_percentage, uint16_t interval_ms){
    if (this->time_last_change + interval_ms <= millis()){
        if (this->current_percentage < target_percentage){
            //increase value
            u_int8_t new_percentage = this->current_percentage + 2;
            if (new_percentage > target_percentage){
                new_percentage = target_percentage;
            }
            this->set_level_logarithmic_percent(new_percentage);
        }else if (this->current_percentage > target_percentage){
            //decrease value
            u_int8_t new_percentage = this->current_percentage - 2;
            if (new_percentage < target_percentage){
                new_percentage = target_percentage;
            }
            this->set_level_logarithmic_percent(new_percentage);
        }
    }
    return this->get_value() == this->value;
}

void AnalogOutput::fade_logarithmic_between_percentages(u_int8_t percent_1, u_int8_t percent_2, uint16_t interval_1_ms, uint16_t interval_2_ms, uint64_t wait_time_1_ms, uint64_t wait_time_2_ms){
    if (!this->fade_animation_step){
        if (this->current_percentage != percent_1){ //fade to percentage 1
            this->fade_logarithmic_to_percent(percent_1, interval_1_ms);
        }else{
            if (this->time_last_change + wait_time_1_ms <= millis()){
                this->fade_animation_step = true;
            }
        }
    }else{
        if (this->current_percentage != percent_2){ //fade to percentage 2
            this->fade_logarithmic_to_percent(percent_2, interval_2_ms);
        }else{
            if (this->time_last_change + wait_time_2_ms <= millis()){
                this->fade_animation_step = false;
            }
        }
    }
}