#include "LedDisplay.hpp"

LedDisplay::LedDisplay(uint8_t green_led_pin, uint8_t red_led_pin, uint8_t blue_led_pin) 
: green_led(green_led_pin), red_led(red_led_pin), blue_led(blue_led_pin) {}



bool LedDisplay::run_startup_animation(){
    const uint8_t MAX_PERCENTAGE = 50;
    const uint8_t SWITCH_TO_NEXT_PERCENTAGE_UP = 16;
    const uint8_t SWITCH_TO_NEXT_PERCENTAGE_DOWN = MAX_PERCENTAGE - SWITCH_TO_NEXT_PERCENTAGE_UP;
    const uint16_t INTERVAL_MS = 12;
    const uint8_t LOOPS = 3;

    if (!this->startup_animation_finished){
        switch (this->startup_animation_step % 2)
        {
        case 0: //fade leds up
            this->green_led.fade_logarithmic_to_percent(MAX_PERCENTAGE, INTERVAL_MS);
            if (this->green_led.get_current_percentage() > SWITCH_TO_NEXT_PERCENTAGE_UP){
                this->red_led.fade_logarithmic_to_percent(MAX_PERCENTAGE, INTERVAL_MS);
            }
            if (this->red_led.get_current_percentage() > SWITCH_TO_NEXT_PERCENTAGE_UP){
                this->blue_led.fade_logarithmic_to_percent(MAX_PERCENTAGE, INTERVAL_MS);
            }
            if (this->blue_led.get_current_percentage() >= MAX_PERCENTAGE){
                this->startup_animation_step++;
                Serial.printf("Startup Animation %d %%\n", this->startup_animation_step * 100 / (LOOPS * 2));
            }
            break;
        case 1: //fade leds down
            this->green_led.fade_logarithmic_to_percent(0, INTERVAL_MS);
            if (this->green_led.get_current_percentage() < SWITCH_TO_NEXT_PERCENTAGE_DOWN){
                this->red_led.fade_logarithmic_to_percent(0, INTERVAL_MS);
            }
            if (this->red_led.get_current_percentage() < SWITCH_TO_NEXT_PERCENTAGE_DOWN){
                this->blue_led.fade_logarithmic_to_percent(0, INTERVAL_MS);
            }
            if (this->blue_led.get_current_percentage() == 0){
                this->startup_animation_step++;
                Serial.printf("Startup Animation %d %%\n", this->startup_animation_step * 100 / (LOOPS * 2));
            }
            break;
        default:
            break;
        }
        this->startup_animation_finished = this->startup_animation_step > LOOPS * 2 - 1;
    }
    return this->startup_animation_finished;
}

void LedDisplay::display_state(Pump *pump, DigitalInput *waterlevel_sensor, OneWireTemperatureSensor *temperature_sensor, bool wifi_connected, bool mqtt_connected){
    //green led -> pump state
    if (pump->get_state() && pump->get_enabled()){
        this->green_led.fade_logarithmic_between_percentages(30, 100, 15, 15, 250, 250); //pump on
    }else if (!pump->get_state() && pump->get_enabled()){
        this->green_led.fade_logarithmic_to_percent(30, 15); //pump off (ready)
    }else{
        this->green_led.fade_logarithmic_to_percent(0, 10); //pump disabled
    }
    
    //red led -> pump disabled / waterlevel / error state
    if (!pump->get_enabled()){
        this->red_led.fade_logarithmic_to_percent(30, 10); //pump disabled
    }else{
        if (waterlevel_sensor->get_state()){
            this->red_led.fade_logarithmic_between_percentages(0, 100, 30, 10, 300, 200); //water level low
        }else{
            bool error = temperature_sensor->get_error(); 
            if (error){
                this->red_led.fade_logarithmic_between_percentages(0, 100, 2, 2, 250, 250); //any error
            }else{
                this->red_led.fade_logarithmic_to_percent(0, 10); //no error
            }
        }
    }

    //blue led -> connectivity state
    if (wifi_connected && mqtt_connected){
        this->blue_led.fade_logarithmic_to_percent(30); //all connected
    }else if (wifi_connected && !mqtt_connected){
        this->blue_led.fade_logarithmic_between_percentages(30, 100, 8, 8, 100, 100); //wifi connected, mqtt not
    }else{
        this->blue_led.fade_logarithmic_between_percentages(0, 100, 30, 30, 500, 500); //no connection
    }
}
