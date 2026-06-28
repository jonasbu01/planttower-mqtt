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
    float temperature = 0.0;
    bool error = false;
    uint64_t time_last_request = 0;
    float valid_temperature = 0.0;
    uint64_t time_last_valid_temperature = 0;
    uint64_t DURATION_VALID_TEMPERATUR_S = 900; //15 min
public:
    OneWireTemperatureSensor(u_int8_t pin);
    void request_value();
    void request_value_by_interval(u_int16_t interval_s);
    float get_temperature();
    bool get_error();
};

#endif