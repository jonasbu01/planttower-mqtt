#include "MqttEntity.hpp"

const char* MqttEntity::get_unique_id() {
  return this->unique_id;
}

const char* MqttEntity::get_state_topic() {
  return this->state_topic;
}