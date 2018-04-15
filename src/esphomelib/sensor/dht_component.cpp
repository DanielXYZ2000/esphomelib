//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/sensor/dht_component.h"

#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/helpers.h"
#include "esphomelib/espmath.h"

#ifdef USE_DHT_SENSOR

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor.dht";

DHTComponent::DHTComponent(const std::string &temperature_name, const std::string &humidity_name,
                           uint8_t pin, uint32_t update_interval)
    : temperature_sensor_(new DHTTemperatureSensor(temperature_name, this)),
      humidity_sensor_(new DHTHumiditySensor(humidity_name, this)),
      PollingComponent(update_interval) {
  this->set_pin(pin);
}

void DHTComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DHT...");
  ESP_LOGCONFIG(TAG, "    Pin: %u", this->pin_);
  ESP_LOGCONFIG(TAG, "    Model: %u", this->model_);
  this->dht_.setup(this->pin_, this->model_);
  if (this->model_ == DHT::AUTO_DETECT)
    ESP_LOGD(TAG, "    DHT.setup() -> status: %u", this->dht_.getStatus());
}

void DHTComponent::update() {
  auto temp_hum = run_without_interrupts<std::pair<float, float>>([this] {
    return std::make_pair(this->dht_.getTemperature(), this->dht_.getHumidity());
  });
  float temperature = temp_hum.first;
  float humidity = temp_hum.second;

  if (temperature == 0.0 && humidity == 1.0) {
    // Sometimes DHT22 queries fail in a weird way where temperature is exactly 0.0 °C
    // and humidity is exactly 1%. This *exact* value pair shouldn't really happen
    // that much out in the wild (unless you're in a very cold dry room).
    // FIXME
    ESP_LOGW(TAG, "Got invalid temperature %f°C and humidity %f%% pair.");
    return;
  }

  ESP_LOGD(TAG, "Got Temperature=%.1f°C Humidity=%.1f%%", temperature, humidity);

  if (!isnan(temperature))
    this->temperature_sensor_->push_new_value(temperature);
  else
    ESP_LOGW(TAG, "Invalid Temperature: %f!C", temperature);

  if (!isnan(humidity))
    this->humidity_sensor_->push_new_value(humidity);
  else
    ESP_LOGW(TAG, "Invalid Humidity: %f%%", humidity);
}

float DHTComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
uint8_t DHTComponent::get_pin() const {
  return this->pin_;
}
void DHTComponent::set_pin(uint8_t pin) {
  assert_is_pin(pin);
  assert_construction_state(this);
  this->pin_ = pin;
}
void DHTComponent::set_dht_model(DHT::DHT_MODEL_t model) {
  assert_construction_state(this);
  this->model_ = model;
}
DHT &DHTComponent::get_dht() {
  return this->dht_;
}
DHTTemperatureSensor *DHTComponent::get_temperature_sensor() const {
  return this->temperature_sensor_;
}
DHTHumiditySensor *DHTComponent::get_humidity_sensor() const {
  return this->humidity_sensor_;
}
std::string DHTTemperatureSensor::unit_of_measurement() {
  return "°C";
}
std::string DHTTemperatureSensor::icon() {
  return "";
}
uint32_t DHTTemperatureSensor::update_interval() {
  assert(this->parent_ != nullptr);

  return this->parent_->get_update_interval();
}
DHTTemperatureSensor::DHTTemperatureSensor(const std::string &name, DHTComponent *parent)
    : Sensor(name), parent_(parent) {}

int8_t DHTTemperatureSensor::accuracy_decimals() {
  return this->parent_->get_dht().getNumberOfDecimalsTemperature();
}

std::string DHTHumiditySensor::unit_of_measurement() {
  return "%";
}
std::string DHTHumiditySensor::icon() {
  return "mdi:water-percent";
}
uint32_t DHTHumiditySensor::update_interval() {
  assert(this->parent_ != nullptr);

  return this->parent_->get_update_interval();
}
DHTHumiditySensor::DHTHumiditySensor(const std::string &name, DHTComponent *parent)
    : Sensor(name), parent_(parent) {}

int8_t DHTHumiditySensor::accuracy_decimals() {
  return this->parent_->get_dht().getNumberOfDecimalsHumidity();
}

} // namespace sensor

} // namespace esphomelib

#endif //USE_DHT_SENSOR
