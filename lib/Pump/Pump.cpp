#include "Pump.hpp"

Pump::Pump(uint8_t pump_pin, MqttSwitch* mqtt_switch)
    : DigitalOutput(pump_pin) {
        this->mqtt_switch = mqtt_switch;
        this->mqtt_switch->on_state_change([this](const char* state) {
            if (strcmp(state, MqttSwitch::ON_STATE) == 0) {
                if (!this->get_state()) {
                    this->switch_on();
                }
            } else if (strcmp(state, MqttSwitch::OFF_STATE) == 0) {
                if (this->get_state()) {
                    this->switch_off();
                }
            }
        });
    }

void Pump::run_interval_cycle(OneWireTemperatureSensor *temperature_sensor, uint64_t on_duration_s, uint64_t off_duration_below_20C_s) {
    if (temperature_sensor->get_temperature() < 2.0 && !temperature_sensor->get_error()){ //ice preventation
        if (this->get_state()){
            this->switch_off();
        }
        this->duration_until_on_s = 0;
        this->duration_until_off_s = 0;
    }else{ 
        if(!this->get_state()) { //off -> switch on
            float off_duration_s = this->calculate_off_duration(temperature_sensor, off_duration_below_20C_s);
            this->duration_until_on_s = (this->get_time_switched_off() + off_duration_s * 1000 - millis()) / 1000;
            this->duration_until_off_s = 0;
            if (this->duration_until_on_s <= 0) {
                this->switch_on();
                this->mqtt_switch->switch_on();
            }
        }
        if (this->get_state()){ //on -> switch off
            this->duration_until_off_s = (this->get_time_switched_on() + on_duration_s * 1000 - millis()) / 1000;
            this->duration_until_on_s = 0;
            if(this->duration_until_off_s <= 0) { 
                this->switch_off();
                this->mqtt_switch->switch_off();
            }
        }
    }
}

uint64_t Pump::calculate_off_duration(OneWireTemperatureSensor *temperature_sensor, uint64_t off_duration_below_20C_s){
    if (temperature_sensor->get_error() || temperature_sensor->get_temperature() <= 20.0){
        return off_duration_below_20C_s;
    }else{
        uint8_t kelvin_over_20C = (int(temperature_sensor->get_temperature()) - 20);
        //float factor = 1 - (1 - 0.3)/15 * kelvin_over_20C; //@15 K (40°C) -> 0.3 * off_duration_25C_s
        float factor = 1 - 0.9 * pow(kelvin_over_20C / 15.0, 2) - kelvin_over_20C * 0.01;
        if (factor < 0.1){
            factor = 0.1; //higher temp. than 34°C -> 4 min
        }
        uint64_t warm_temperature_off_duration = off_duration_below_20C_s * factor;
        return warm_temperature_off_duration;
    }
}

int64_t Pump::get_duration_until_on_s(){
    return this->duration_until_on_s;
}

int64_t Pump::get_duration_until_off_s(){
    return this->duration_until_off_s;
}
