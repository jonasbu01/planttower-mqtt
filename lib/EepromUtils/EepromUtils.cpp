#include "EepromUtils.hpp"

static bool eeprom_initialized = false; //only one-time initialization!
static uint16_t eeprom_size = 256;

static const char* byteToBinary(uint8_t value) {
    static char buffer[9];
    for (int i = 7; i >= 0; i--) {
        buffer[7 - i] = (value & (1 << i)) ? '1' : '0';
    }
    buffer[8] = '\0';
    return buffer;
}

void eeprom_begin(uint16_t size) {
    eeprom_size = size;
    if (!eeprom_initialized) {
        EEPROM.begin(eeprom_size);
        eeprom_initialized = true;
    }
}

bool eeprom_read_bool(uint8_t byteAddr, uint8_t bit) {
    if (!eeprom_initialized) return false;
    if (byteAddr >= eeprom_size || bit > 7) return false;

    uint8_t val = EEPROM.read(byteAddr);
    if (val == 0xFF) return false; // uninitialisiert → default false

    return (val >> bit) & 0x01;
}

void eeprom_write_bool(uint8_t byteAddr, uint8_t bit, bool value) {
    if (!eeprom_initialized) return;
    if (byteAddr >= eeprom_size || bit > 7) return;

    uint8_t oldVal = EEPROM.read(byteAddr);
    if (oldVal == 0xFF) oldVal = 0; // uninitialisiert → auf 0 setzen

    uint8_t newVal = oldVal;

    if (value) {
        newVal |= (1 << bit);   // Bit setzen
    } else {
        newVal &= ~(1 << bit);  // Bit löschen
    }

    EEPROM.write(byteAddr, newVal);
    EEPROM.commit();

    Serial.println(F("==== EEPROM WRITE ===="));
    Serial.printf("Address : %u\n", byteAddr);
    Serial.printf("Bit     : %u\n", bit);
    Serial.printf("Value   : %s\n", value ? "true" : "false");
    Serial.printf("Old byte: 0x%02X (%s)\n", oldVal, byteToBinary(oldVal));
    Serial.printf("New byte: 0x%02X (%s)\n", newVal, byteToBinary(newVal));
    Serial.println(F("======================"));
}
