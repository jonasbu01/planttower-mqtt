#ifndef ONEWIRETEMPERATURESENSOR
#define ONEWIRETEMPERATURESENSOR

#include "Arduino.h"
#include "OneWire.h"
//#include "OneWireESP32.h"
#include "DallasTemperature.h"

class OneWireTemperatureSensor {
private:
    OneWire one_wire;
    //OneWireESP32 one_wire;
    DallasTemperature sensor;
    float temperature = 0;
    bool error = false;
    uint64_t time_last_request = 0;
public:
    OneWireTemperatureSensor(u_int8_t pin);
    void request_value();
    void request_value_by_interval(u_int16_t interval_s);
    float get_temperature();
    bool get_error();
};

#endif