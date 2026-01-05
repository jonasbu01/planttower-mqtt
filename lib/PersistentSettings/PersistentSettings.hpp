#ifndef PERSISTENT_SETTINGS
#define PERSISTENT_SETTINGS

#include <SettingsManagerESP32.h>

#define BOOLS(X) \
  X(None, "None", false, false) \
  X(ConnectionTested, "Wifi and MQTT was tested", false, false) \
  X(PumpEnabled, "Is pump enabled", true, false)

#define INTEGERS(X) \
  X(None, "None", 0, false) \
  X(MQTTPORT, "MQTT Port", 1883, false)

#define STRINGS(X) \
  X(None, "None", "", true) \
  X(WIFISSID, "Wifi SSID", "", true) \
  X(WIFIPASSWORD, "Wifi Password", "", true) \
  X(MQTTSERVER, "MQTT Server", "", true) \
  X(MQTTUSER, "MQTT User", "", true) \
  X(MQTTPASSWORD, "MQTT Password", "", true) \
  X(MQTTDEVICENAME, "MQTT Device Name", "", true) \
  X(MQTTDEVICEID, "MQTT Device ID", "", true)

enum class Bools : uint8_t { BOOLS(SETTINGS_EXPAND_ENUM_CLASS) };
enum class Integers : uint8_t { INTEGERS(SETTINGS_EXPAND_ENUM_CLASS) };
enum class Strings : uint8_t { STRINGS(SETTINGS_EXPAND_ENUM_CLASS) };

extern NVS::Settings<bool, Bools, SETTINGS_COUNT(BOOLS)> bool_settings;
extern NVS::Settings<int, Integers, SETTINGS_COUNT(INTEGERS)> integer_settings;
extern NVS::Settings<const char*, Strings, SETTINGS_COUNT(STRINGS)> string_settings;
bool getStringSettingSafe(Strings key, char* buffer, size_t bufSize);

#endif
