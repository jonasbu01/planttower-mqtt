#include "PersistentSettings.hpp"

NVS::Settings<bool, Bools, SETTINGS_COUNT(BOOLS)> bool_settings = {BOOLS(SETTINGS_EXPAND_SETTINGS)};
NVS::Settings<int, Integers, SETTINGS_COUNT(INTEGERS)> integer_settings = {INTEGERS(SETTINGS_EXPAND_SETTINGS)};
NVS::Settings<const char*, Strings, SETTINGS_COUNT(STRINGS)> string_settings = {STRINGS(SETTINGS_EXPAND_SETTINGS)};

bool getStringSettingSafe(Strings key, char* buffer, size_t bufSize) {
    if (!buffer || bufSize == 0) return false;

    const char* raw = string_settings.getValue(key);
    if (!raw) {
        buffer[0] = '\0';
        string_settings.giveMutex();
        return false;
    }

    strncpy(buffer, raw, bufSize - 1);
    buffer[bufSize - 1] = '\0';
    string_settings.giveMutex();
    return true;
}