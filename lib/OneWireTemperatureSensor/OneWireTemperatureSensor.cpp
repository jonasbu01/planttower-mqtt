#include "OneWireTemperatureSensor.hpp"

OneWireTemperatureSensor::OneWireTemperatureSensor(u_int8_t pin) 
    : one_wire(pin), sensor(&this->one_wire){
        this->sensor.begin();
}

void OneWireTemperatureSensor::request_value(){
    this->sensor.requestTemperatures();
    float sensor_value = this->sensor.getTempCByIndex(0);
    this->time_last_request = millis();
    if (sensor_value > 70.0 || sensor_value < -30.0){
        this->error = true;
        this->sensor.begin();
    }else{
        this->error = false;
        this->temperature = sensor_value;
    }
}

void OneWireTemperatureSensor::request_value_by_interval(u_int16_t interval_s){
    if (this->time_last_request + interval_s * 1000 <= millis()){
        this->request_value();
    }
}

float OneWireTemperatureSensor::get_temperature(){
    return this->temperature;
}

bool OneWireTemperatureSensor::get_error(){
    return this->error;
}