#include "OneWireTemperatureSensor.hpp"

OneWireTemperatureSensor::OneWireTemperatureSensor(u_int8_t pin) 
    : one_wire(pin), sensor(&this->one_wire){
        this->sensor.begin();
}

void OneWireTemperatureSensor::request_value(){
    //request
    this->sensor.requestTemperatures();
    float sensor_value = this->sensor.getTempCByIndex(0);
    this->time_last_request = millis();
    this->temperature = sensor_value;
    //check if valid
    if (sensor_value > 70.0 || sensor_value < -30.0 || sensor_value == 0.0){
        this->sensor.begin();
    }else{
        this->valid_temperature = sensor_value;
        this->time_last_valid_temperature = this->time_last_request;
    }
    //error if no valid temperature for more than DURATION_VALID_TEMPERATUR_S
    if (this->time_last_valid_temperature + this->DURATION_VALID_TEMPERATUR_S * 1000 < this->time_last_request){
        this->error = true;
    }else{
        this->error = false;
    }
}

void OneWireTemperatureSensor::request_value_by_interval(u_int16_t interval_s){
    if (this->time_last_request + interval_s * 1000 <= millis()){
        this->request_value();
    }
}

float OneWireTemperatureSensor::get_temperature(){
    return this->valid_temperature;
}

bool OneWireTemperatureSensor::get_error(){
    return this->error;
}