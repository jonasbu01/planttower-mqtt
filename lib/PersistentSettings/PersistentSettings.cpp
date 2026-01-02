#include "PersistentSettings.hpp"

NVS::Settings<bool, Bools, SETTINGS_COUNT(BOOLS)> bool_settings = {BOOLS(SETTINGS_EXPAND_SETTINGS)};
