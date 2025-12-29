#include "LedDisplay.hpp"

LedDisplay::LedDisplay(uint8_t green_led_pin, uint8_t red_led_pin, uint8_t blue_led_pin) 
: green_led(green_led_pin), red_led(red_led_pin), blue_led(blue_led_pin) {}

bool LedDisplay::run_startup_animation(){
    const uint8_t MAX_LEVEL = 60;
    const uint8_t SWITCH_TO_NEXT_LEVEL_UP = 15;
    const uint8_t SWITCH_TO_NEXT_LEVEL_DOWN = MAX_LEVEL - SWITCH_TO_NEXT_LEVEL_UP;
    const uint8_t LOOPS = 3;

    if (!this->startup_animation_finished){
        switch (this->startup_animation_step % 2)
        {
        case 0: //fade leds up
            this->green_led.increase_to_target_value(MAX_LEVEL);
            if (this->green_led.get_value() > SWITCH_TO_NEXT_LEVEL_UP){
                this->red_led.increase_to_target_value(MAX_LEVEL);
            }
            if (this->red_led.get_value() > SWITCH_TO_NEXT_LEVEL_UP){
                if(this->blue_led.increase_to_target_value(MAX_LEVEL)){
                    this->startup_animation_step++;
                }
            }
            break;
        case 1: //fade leds down
            this->green_led.decrease_to_target_value(0);
            if (this->green_led.get_value() < SWITCH_TO_NEXT_LEVEL_DOWN){
                this->red_led.decrease_to_target_value(0);
            }
            if (this->red_led.get_value() < SWITCH_TO_NEXT_LEVEL_DOWN){
                if(this->blue_led.decrease_to_target_value(0)){
                    this->startup_animation_step++;
                }
            }
            break;
        default:
            break;
        }
        this->startup_animation_finished = this->startup_animation_step > LOOPS * 2 - 1;
    }
    return this->startup_animation_finished;
}

void LedDisplay::display_state(Pump *pump, DigitalInput *waterlevel_sensor, OneWireTemperatureSensor *temperature_sensor){
    //pump - green led
    if (pump->get_state()){
        this->green_led.fade_between_two_values(255, 30, 2, 2, 1, 1, 30, 30);
    }else{
        this->green_led.fade_to_target_value(60);
    }
    //water level and errors -> red led
    if (waterlevel_sensor->get_state()){
        this->red_led.fade_between_two_values(255, 0, 2, 5, 1, 1, 150, 100); //water level low
    }else{
        bool error = temperature_sensor->get_error(); 
        if (error){
            this->red_led.fade_between_two_values(255, 0, 1, 1, 3, 3, 120, 150); //any error
        }else{
            this->red_led.fade_to_target_value(0); //no error
        }
    }
    //zigbee - blue led
    this->blue_led.fade_between_two_values(120, 0, 20, 20, 1, 1, 50, 2000); //search signal
}
