//
// Created by Otto Winter on 25.11.17.
//

#include <algorithm>
#include "esphomelib/component.h"

#include "esphomelib/esphal.h"
#include "esphomelib/log.h"
#include "esphomelib/helpers.h"

namespace esphomelib {

static const char *TAG = "component";

float Component::get_loop_priority() const {
  return 0.0f;
}

float Component::get_setup_priority() const {
  return 0.0f;
}

void Component::setup() {

}

void Component::loop() {

}

void Component::set_interval(const std::string &name, uint32_t interval, time_func_t &&f) {
  uint32_t offset = random_uint32() % interval;
  ESP_LOGV(TAG, "set_interval(name='%s', interval=%u, offset=%u)", name.c_str(), interval, offset);

  this->cancel_interval(name);
  struct TimeFunction function = {
      .name = name,
      .type = TimeFunction::INTERVAL,
      .interval = interval,
      .last_execution = millis() + offset,
      .f = std::move(f),
      .remove = false,
  };
  this->time_functions_.push_back(function);
}

bool Component::cancel_interval(const std::string &name) {
  return this->cancel_time_function(name, TimeFunction::INTERVAL);
}

void Component::set_timeout(const std::string &name, uint32_t timeout, time_func_t &&f) {
  ESP_LOGV(TAG, "set_timeout(name='%s', timeout=%u)", name.c_str(), timeout);

  this->cancel_timeout(name);
  struct TimeFunction function = {
      .name = name,
      .type = TimeFunction::TIMEOUT,
      .interval = timeout,
      .last_execution = millis(),
      .f = std::move(f),
      .remove = false,
  };
  this->time_functions_.push_back(function);
}

bool Component::cancel_timeout(const std::string &name) {
  return this->cancel_time_function(name, TimeFunction::TIMEOUT);
}

void Component::loop_() {
  this->loop_internal();
  this->loop();
}

bool Component::cancel_time_function(const std::string &name, TimeFunction::Type type) {
  if (name.empty())
    return false;
  for (auto iter = this->time_functions_.begin(); iter != this->time_functions_.end(); iter++) {
    if (!iter->remove && iter->name == name && iter->type == type) {
      ESP_LOGV(TAG, "Removing old time function %s.", iter->name.c_str());
      iter->remove = true;
      return true;
    }
  }
  return false;
}
void Component::setup_() {
  this->setup_internal();
  this->setup();
}
Component::ComponentState Component::get_component_state() const {
  return this->component_state_;
}
void Component::loop_internal() {
  assert_setup(this);
  this->component_state_ = LOOP;

  for (int i = 0; i < this->time_functions_.size(); i++) { // NOLINT
    TimeFunction *tf = &this->time_functions_[i];
    if (tf->should_run()) {
      tf->f();

      if (tf->type == TimeFunction::INTERVAL) {
        uint32_t amount = (millis() - tf->last_execution) / tf->interval;
        tf->last_execution += amount * tf->interval;
      } else if (tf->type == TimeFunction::DEFER || tf->type == TimeFunction::TIMEOUT) {
        tf->remove = true;
      }
    }
  }

  this->time_functions_.erase(
      std::remove_if(this->time_functions_.begin(), this->time_functions_.end(),
                     [](const TimeFunction &tf) -> bool {
                       return tf.remove;
                     }),
      this->time_functions_.end()
  );
}
void Component::setup_internal() {
  assert_construction_state(this);
  this->component_state_ = SETUP;
}
void Component::mark_failed() {
  ESP_LOGE(TAG, "Component was marked as failed.");
  this->component_state_ = FAILED;
}
void Component::defer(Component::time_func_t &&f) {
  this->defer("", std::move(f));
}
bool Component::cancel_defer(const std::string &name) {
  return this->cancel_time_function(name, TimeFunction::DEFER);
}
void Component::defer(const std::string &name, Component::time_func_t &&f) {
  ESP_LOGV(TAG, "defer(name='%s')", name.c_str());

  this->cancel_defer(name);
  struct TimeFunction function = {
      .name = name,
      .type = TimeFunction::DEFER,
      .interval = 0,
      .last_execution = 0,
      .f = std::move(f),
      .remove = false,
  };
  this->time_functions_.push_back(function);
}
void Component::set_timeout(uint32_t timeout, Component::time_func_t &&f) {
  this->set_timeout("", timeout, std::move(f));
}
void Component::set_interval(uint32_t interval, Component::time_func_t &&f) {
  this->set_interval("", interval, std::move(f));
}

PollingComponent::PollingComponent(uint32_t update_interval)
    : Component(), update_interval_(update_interval) {}

void PollingComponent::setup_() {
  // Call component internal setup.
  this->setup_internal();

  // Let the polling component subclass setup their HW.
  this->setup();

  // Register interval.
  ESP_LOGCONFIG(TAG, "    Update interval: %ums", this->get_update_interval());
  this->set_interval("update", this->get_update_interval(), [this]() { this->update(); });
}

uint32_t PollingComponent::get_update_interval() const {
  return this->update_interval_;
}
void PollingComponent::set_update_interval(uint32_t update_interval) {
  this->update_interval_ = update_interval;
}

const std::string &Nameable::get_name() const {
  return this->name_;
}
void Nameable::set_name(const std::string &name) {
  this->name_ = name;
}
Nameable::Nameable(const std::string &name)
    : name_(name) {}
const std::string &Nameable::get_name_id() {
  if (!this->name_id_.empty())
    return this->name_id_;
  this->name_id_ = sanitize_string_whitelist(to_lowercase_underscore(this->name_), HOSTNAME_CHARACTER_WHITELIST);
  return this->name_id_;
}

bool Component::TimeFunction::should_run() const {
  if (this->remove)
    return false;
  if (this->type == DEFER)
    return true;
  return millis() - this->last_execution > this->interval;
}

} // namespace esphomelib
