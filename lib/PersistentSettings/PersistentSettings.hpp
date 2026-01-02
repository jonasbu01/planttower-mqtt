#ifndef PERSISTENT_SETTINGS
#define PERSISTENT_SETTINGS

#include <SettingsManagerESP32.h>

#define BOOLS(X) \
  X(None, "None", false, false) \
  X(PumpEnabled, "Is pump enabled", true, false)

enum class Bools : uint8_t { BOOLS(SETTINGS_EXPAND_ENUM_CLASS) };

extern NVS::Settings<bool, Bools, SETTINGS_COUNT(BOOLS)> bool_settings;

#endif
