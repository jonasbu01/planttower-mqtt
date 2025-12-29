#include "AnalogInput.hpp"

AnalogInput::AnalogInput(u_int8_t pin){
    this->pin = pin;
    pinMode(this->pin, INPUT);
}

void AnalogInput::sample_each_ms(){
    if (this->time_last_sample != millis()){
        this->raw_value = analogRead(this->pin);
        this->raw_samples.push_back(this->raw_value);
        if (this->raw_samples.size() > this->SAMPLE_AMOUNT_FILTER) {
            this->raw_samples.pop_front();
        }
    }
}

float AnalogInput::calc_voltage(u_int16_t value){
    return (float)value / (float)this->ADC_BITS * this->ADC_VOLTAGE_REFERENCE;
}

u_int16_t AnalogInput::get_raw_value(){
    return this->raw_value;
}

float AnalogInput::get_voltage(){
    return this->calc_voltage(this->raw_value);
}

float AnalogInput::get_filtered_voltage(){
    this->update_filtered_value();
    return this->calc_voltage(this->filtered_value);
}

void AnalogInput::update_filtered_value() {
    if (!this->raw_samples.empty()) {
        //sort, delete amount of min / max values
        std::deque<u_int16_t> sorted_samples = this->raw_samples;
        if (this->raw_samples.size() >= this->SAMPLE_AMOUNT_FILTER){
            std::sort(sorted_samples.begin(), sorted_samples.end());
            sorted_samples.erase(sorted_samples.begin(), sorted_samples.begin() + this->SAMPLE_AMOUNT_DELETE_MIN_MAX / 2);
            sorted_samples.erase(sorted_samples.end() - this->SAMPLE_AMOUNT_DELETE_MIN_MAX / 2, sorted_samples.end());
        }
        //average
        uint64_t sum = 0;
        for (u_int16_t sample : sorted_samples) {
            sum += sample;
        }
        this->filtered_value = sum / sorted_samples.size();
    }else{
        this->filtered_value = this->raw_value;
    }
}