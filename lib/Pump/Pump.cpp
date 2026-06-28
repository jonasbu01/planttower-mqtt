#include "Pump.hpp"

Pump::Pump(uint8_t pump_pin, MqttSwitch* mqtt_pump_enable_switch, MqttSwitch* mqtt_pump_switch)
    : DigitalOutput(pump_pin) {
        this->mqtt_pump_enable_switch = mqtt_pump_enable_switch;
        this->mqtt_pump_enable_switch->on_state_change([this](const char* state) {
            if (strcmp(state, MqttSwitch::ON_STATE) == 0) {
                this->enabled = true;
                this->mqtt_pump_switch->switch_on();
            } else if (strcmp(state, MqttSwitch::OFF_STATE) == 0) {
                this->enabled = false;
                this->mqtt_pump_switch->switch_off();
            }
        });

        this->mqtt_pump_switch = mqtt_pump_switch;
        this->mqtt_pump_switch->on_state_change([this](const char* state) {
            if (strcmp(state, MqttSwitch::ON_STATE) == 0) {
                if (!this->get_state() && this->enabled) {
                    this->switch_on();
                } else if (!this->enabled) {
                    this->mqtt_pump_switch->switch_off();
                }
            } else if (strcmp(state, MqttSwitch::OFF_STATE) == 0) {
                if (this->get_state()) {
                    this->switch_off();
                }
            }
        });
    }

void Pump::run_interval_cycle(OneWireTemperatureSensor *temperature_sensor, uint64_t on_duration_s, uint64_t off_duration_below_20C_s) {
    if (!this->enabled){ // pump disabled
        if (this->get_state()){
            this->mqtt_pump_switch->switch_off();
        }
    } else { 
        if (!this->get_state()) { //off -> switch on
            float off_duration_s = this->calculate_off_duration(temperature_sensor, off_duration_below_20C_s);
            this->duration_until_on_s = (this->get_time_switched_off() + off_duration_s * 1000 - millis()) / 1000;
            this->duration_until_off_s = 0;
            if (this->duration_until_on_s <= 0) {
                this->mqtt_pump_switch->switch_on();
            }
        }
        if (this->get_state()) { //on -> switch off
            this->duration_until_off_s = (this->get_time_switched_on() + on_duration_s * 1000 - millis()) / 1000;
            this->duration_until_on_s = 0;
            if (this->duration_until_off_s <= 0) { 
                this->mqtt_pump_switch->switch_off();
            }
        }
    }
}

uint64_t Pump::calculate_off_duration(OneWireTemperatureSensor *temperature_sensor, uint64_t off_duration_below_20C_s){
    float temperature = temperature_sensor->get_temperature();
    if (temperature_sensor->get_error() || temperature <= 20.0){
        return off_duration_below_20C_s;
    }else{
        uint8_t kelvin_over_20C = (int(temperature) - 20);
        //float factor = 1 - (1 - 0.3)/15 * kelvin_over_20C; //@15 K (40°C) -> 0.3 * off_duration_25C_s
        float factor = 1 - 0.9 * pow(kelvin_over_20C / 15.0, 2) - kelvin_over_20C * 0.01;
        if (factor < 0.15){
            factor = 0.15; //min time 6 min
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

int64_t Pump::get_duration_until_change_s(){
    if (this->get_state()){
        return this->duration_until_off_s;
    }else{
        return this->duration_until_on_s;
    }
}

void Pump::toggle_enabled() {
    if (this->enabled) {
        this->mqtt_pump_enable_switch->switch_off();
        this->enabled = false;
    } else {
        this->mqtt_pump_enable_switch->switch_on();
        this->enabled = true;
    }
}

bool Pump::get_enabled() {
    return this->enabled;
}
