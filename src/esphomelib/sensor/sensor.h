//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_SENSOR_SENSOR_H
#define ESPHOMELIB_SENSOR_SENSOR_H

#include <functional>

#include "esphomelib/component.h"

namespace esphomelib {

namespace sensor {

using sensor_callback_t = std::function<void(float, int8_t)>;

/** Base-class for all sensors.
 *
 * A sensor has unit of measurement and can use push_new_value to send out a new value with the specified accuracy.
 */
class Sensor {
 public:
  explicit Sensor(uint32_t update_interval);

  virtual std::string unit_of_measurement() = 0;

  void set_new_value_callback(sensor_callback_t callback);

  void push_new_value(float value, int8_t accuracy_decimals);

  virtual uint32_t get_update_interval() const;
  virtual void set_update_interval(uint32_t update_interval);

 protected:
  sensor_callback_t callback_{nullptr};
  uint32_t update_interval_{0};
};

class TemperatureSensor : public Sensor {
 public:
  explicit TemperatureSensor(uint32_t update_interval);
  std::string unit_of_measurement() override;
};

class HumiditySensor : public Sensor {
 public:
  explicit HumiditySensor(uint32_t update_interval);
  std::string unit_of_measurement() override;
};

} // namespace sensor

} // namespace esphomelib


#endif //ESPHOMELIB_SENSOR_SENSOR_H
