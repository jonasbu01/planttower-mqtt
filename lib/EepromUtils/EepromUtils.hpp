#ifndef EEPROM_UTILS_HPP
#define EEPROM_UTILS_HPP

#include <Arduino.h>
#include <EEPROM.h>

void eeprom_begin(uint16_t size = 256);

bool eeprom_read_bool(uint8_t byteAddr, uint8_t bit);
void eeprom_write_bool(uint8_t byteAddr, uint8_t bit, bool value);



#endif