#ifndef PERSISTENT_SETTINGS
#define PERSISTENT_SETTINGS

#include <SettingsManagerESP32.h>

#define BOOLS(X) \
  X(None, "None", false, false) \
  X(APMode, "StartAPMode", false, false) \
  X(ConTested, "ConTested", false, false) \
  X(ConChanged, "ConChange", false, false) \
  X(PumpEnabled, "PumpEnabled", true, false)

#define INTEGERS(X) \
  X(None, "None", 0, false) \
  X(MQTTPORT, "MQTTPort", 1883, false)

#define STRINGS(X) \
  X(None, "None", "", true) \
  X(WIFISSID, "WifiSSID", "", true) \
  X(WIFIPASSWORD, "WifiPassword", "", true) \
  X(MQTTSERVER, "MQTTServer", "", true) \
  X(MQTTUSER, "MQTTUser", "", true) \
  X(MQTTPASSWORD, "MQTTPassword", "", true) \
  X(MQTTDEVICENAME, "MQTTDeviceName", "", true) \
  X(MQTTDEVICEID, "MQTTDeviceID", "", true)

enum class Bools : uint8_t { BOOLS(SETTINGS_EXPAND_ENUM_CLASS) };
enum class Integers : uint8_t { INTEGERS(SETTINGS_EXPAND_ENUM_CLASS) };
enum class Strings : uint8_t { STRINGS(SETTINGS_EXPAND_ENUM_CLASS) };

extern NVS::Settings<bool, Bools, SETTINGS_COUNT(BOOLS)> bool_settings;
extern NVS::Settings<int, Integers, SETTINGS_COUNT(INTEGERS)> integer_settings;
extern NVS::Settings<const char*, Strings, SETTINGS_COUNT(STRINGS)> string_settings;
bool getStringSettingSafe(Strings key, char* buffer, size_t bufSize);

#endif
