//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_APPLICATION_H
#define ESPHOMELIB_APPLICATION_H

#include <vector>
#include "esphomelib/defines.h"
#include "esphomelib/component.h"
#include "esphomelib/controller.h"
#include "esphomelib/debug_component.h"
#include "esphomelib/deep_sleep_component.h"
#include "esphomelib/log.h"
#include "esphomelib/log_component.h"
#include "esphomelib/power_supply_component.h"
#include "esphomelib/ota_component.h"
#include "esphomelib/wifi_component.h"
#include "esphomelib/mqtt/mqtt_client_component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/binary_sensor/gpio_binary_sensor_component.h"
#include "esphomelib/binary_sensor/status_binary_sensor.h"
#include "esphomelib/fan/basic_fan_component.h"
#include "esphomelib/fan/mqtt_fan_component.h"
#include "esphomelib/light/light_output_component.h"
#include "esphomelib/light/mqtt_json_light_component.h"
#include "esphomelib/output/esp8266_pwm_output.h"
#include "esphomelib/output/gpio_binary_output_component.h"
#include "esphomelib/output/ledc_output_component.h"
#include "esphomelib/output/pca9685_output_component.h"
#include "esphomelib/sensor/adc_sensor_component.h"
#include "esphomelib/sensor/ads1115_component.h"
#include "esphomelib/sensor/bmp085_component.h"
#include "esphomelib/sensor/dallas_component.h"
#include "esphomelib/sensor/dht_component.h"
#include "esphomelib/sensor/htu21d_component.h"
#include "esphomelib/sensor/hdc1080_component.h"
#include "esphomelib/sensor/mqtt_sensor_component.h"
#include "esphomelib/sensor/pulse_counter.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/sensor/ultrasonic_sensor.h"
#include "esphomelib/switch_/ir_transmitter_component.h"
#include "esphomelib/switch_/mqtt_switch_component.h"
#include "esphomelib/switch_/simple_switch.h"
#include "esphomelib/switch_/switch.h"
#include "esphomelib/switch_/restart_switch.h"
#include "esphomelib/web_server.h"

namespace esphomelib {

/** This is the class that combines all components.
 *
 * Firstly, this class is used the register component
 */
class Application {
 public:
  /** Set the name of the item that is running this app.
   *
   * Note: This will automatically be converted to lowercase_underscore.
   *
   * @param name The name of your app.
   */
  void set_name(const std::string &name);

  /** Initialize the log system.
   *
   * @param baud_rate The serial baud rate. Set to 0 to disable UART debugging.
   * @param tx_buffer_size The size of the printf buffer.
   * @return The created and initialized LogComponent.
   */
  LogComponent *init_log(uint32_t baud_rate = 115200,
                         size_t tx_buffer_size = 512);

  /** Initialize the WiFi system.
   *
   * Note: for advanced options, such as manual ip, use the return value.
   *
   * @param ssid The ssid of the network you want to connect to.
   * @param password The password of your network. Leave empty for no password
   * @return The WiFiComponent.
   */
  WiFiComponent *init_wifi(const std::string &ssid, const std::string &password = "");

#ifdef USE_OTA
  /** Initialize Over-the-Air updates.
   *
   * @return The OTAComponent. Use this to set advanced settings.
   */
  OTAComponent *init_ota();
#endif

  /** Initialize the MQTT client.
   *
   * @param address The address of your server.
   * @param port The port of your server.
   * @param username The username.
   * @param password The password. Empty for no password.
   * @return The MQTTClient. Use this to set advanced settings.
   */
  mqtt::MQTTClientComponent *init_mqtt(const std::string &address, uint16_t port,
                                       const std::string &username, const std::string &password);

  /** Initialize the MQTT client.
   *
   * @param address The address of your server.
   * @param username The username.
   * @param password The password. Empty for no password.
   * @return The MQTTClient. Use this to set advanced settings.
   */
  mqtt::MQTTClientComponent *init_mqtt(const std::string &address,
                                       const std::string &username, const std::string &password);

#ifdef USE_I2C
    /** Initialize the i2c bus on the provided SDA and SCL pins for use with other components.
     *
     * Note: YOU ONLY NEED TO CALL THIS METHOD ONCE.
     *
     * SDA/SCL pins default to the values defined by the Arduino framework and are usually
     * GPIO4 and GPIO5 on the ESP8266 (D2 and D1 on NodeMCU). And for the ESP32 it defaults to
     * GPIO21 and GPIO22 for SDA and SCL, respectively.
     *
     * If you're unsure about what the defaults are on your board, it's always better
     *
     * @param sda_pin The SDA pin the i2c bus is connected to.
     * @param scl_pin The SCL pin the i2c bus is connected to.
     * @param frequency (only on ESP32) the frequency in Hz the i2c bus should operate at,
     *                  not all components support all frequencies!
     */
  #ifdef ARDUINO_ARCH_ESP32
    void init_i2c(uint8_t sda_pin = SDA, uint8_t scl_pin = SCL, uint32_t frequency = 100000);
  #endif
  #ifdef ARDUINO_ARCH_ESP8266
    void init_i2c(uint8_t sda_pin = SDA, uint8_t scl_pin = SCL);
  #endif
#endif

#ifdef USE_WEB_SERVER
    WebServer *init_web_server(uint16_t port = 80);
#endif





  /*   ____ ___ _   _    _    ______   __  ____  _____ _   _ ____   ___  ____
   *  | __ |_ _| \ | |  / \  |  _ \ \ / / / ___|| ____| \ | / ___| / _ \|  _ \
   *  |  _ \| ||  \| | / _ \ | |_) \ V /  \___ \|  _| |  \| \___ \| | | | |_) |
   *  | |_) | || |\  |/ ___ \|  _ < | |    ___) | |___| |\  |___) | |_| |  _ <
   *  |____|___|_| \_/_/   \_|_| \_\|_|   |____/|_____|_| \_|____/ \___/|_| \_\
   */
#ifdef USE_BINARY_SENSOR
  /// Register a binary sensor and set it up for the front-end.
  binary_sensor::MQTTBinarySensorComponent *register_binary_sensor(binary_sensor::BinarySensor *binary_sensor);
#endif

#ifdef USE_GPIO_BINARY_SENSOR
  struct MakeGPIOBinarySensor {
    binary_sensor::GPIOBinarySensorComponent *gpio;
    binary_sensor::MQTTBinarySensorComponent *mqtt;
  };

  /** Create a simple GPIO binary sensor.
   *
   * Note: advanced options such as inverted input are available in the return value.
   *
   * @param pin The GPIO pin.
   * @param friendly_name The friendly name that should be advertised. Leave empty for no automatic discovery.
   * @param device_class The Home Assistant <a href="https://home-assistant.io/components/binary_sensor/">device_class</a>.
   *                     or esphomelib::binary_sensor::device_class
   */
  MakeGPIOBinarySensor make_gpio_binary_sensor(const std::string &friendly_name,
                                               GPIOInputPin pin,
                                               const std::string &device_class = "");
#endif

#ifdef USE_STATUS_BINARY_SENSOR
  struct MakeStatusBinarySensor {
    binary_sensor::StatusBinarySensor *status;
    binary_sensor::MQTTBinarySensorComponent *mqtt;
  };

  /** Create a simple binary sensor that reports the online/offline state of the node.
   *
   * Uses the MQTT last will and birth message feature. If the values for these features are custom, you need
   * to override them using the return value of this function.
   *
   * @param friendly_name The friendly name advertised via MQTT discovery.
   * @return A MQTTBinarySensorComponent. Use this to set custom status messages.
   */
  MakeStatusBinarySensor make_status_binary_sensor(const std::string &friendly_name);
#endif





  /*   ____  _____ _   _ ____   ___  ____
   *  / ___|| ____| \ | / ___| / _ \|  _ \
   *  \___ \|  _| |  \| \___ \| | | | |_) |
   *   ___) | |___| |\  |___) | |_| |  _ <
   *  |____/|_____|_| \_|____/ \___/|_| \_\
   */
#ifdef USE_SENSOR
  /// Create a MQTTSensorComponent for the provided Sensor and connect them. Mostly for internal use.
  sensor::MQTTSensorComponent *register_sensor(sensor::Sensor *sensor);
#endif

#ifdef USE_DHT_SENSOR
  struct MakeDHTSensor {
    sensor::DHTComponent *dht;
    sensor::MQTTSensorComponent *mqtt_temperature;
    sensor::MQTTSensorComponent *mqtt_humidity;
  };

  /** Create a DHT sensor component.
   *
   * Note: This method automatically applies a SlidingWindowMovingAverageFilter.
   *
   * @param temperature_friendly_name The name the temperature sensor should be advertised as. Leave empty for no
   *                                  automatic discovery.
   * @param humidity_friendly_name The name the humidity sensor should be advertised as. Leave empty for no
   *                                  automatic discovery.
   * @param pin The pin the DHT sensor is connected to.
   * @param update_interval The interval (in ms) the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  MakeDHTSensor make_dht_sensor(const std::string &temperature_friendly_name,
                                const std::string &humidity_friendly_name,
                                uint8_t pin,
                                uint32_t update_interval = 15000);
#endif

#ifdef USE_DALLAS_SENSOR
  sensor::DallasComponent *make_dallas_component(ESPOneWire *one_wire, uint32_t update_interval = 15000);

  sensor::DallasComponent *make_dallas_component(uint8_t pin, uint32_t update_interval = 15000);
#endif

#ifdef USE_PULSE_COUNTER_SENSOR
  struct MakePulseCounterSensor {
    sensor::PulseCounterSensorComponent *pcnt;
    sensor::MQTTSensorComponent *mqtt;
  };

  /** Create an ESP32 Pulse Counter component.
   *
   * The pulse counter peripheral will automatically all pulses on pin in the background. Every
   * check_interval ms the amount of pulses will be retrieved and the difference to the last value
   * will be reported via MQTT as a sensor.
   *
   * @param pin The pin the pulse counter should count pulses on.
   * @param friendly_name The name the sensor should be advertised as.
   * @param update_interval The interval in ms the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  MakePulseCounterSensor make_pulse_counter_sensor(const std::string &friendly_name,
                                                   uint8_t pin,
                                                   uint32_t update_interval = 15000);
#endif

#ifdef USE_ADC_SENSOR
  struct MakeADCSensor {
    sensor::ADCSensorComponent *adc;
    sensor::MQTTSensorComponent *mqtt;
  };

  /** Create an ADC Sensor component.
   *
   * Every check_interval ms, the value from the specified pin (only A0 on ESP8266, 32-39 for ESP32),
   * and converts it into the volt unit. On the ESP32 you can additionally specify a channel attenuation
   * using the return value of this function. pinMode can also be set using the return value.
   *
   * @param pin The pin the ADC should sense on.
   * @param friendly_name The name the sensor should be advertised as.
   * @param update_interval The interval in ms the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  MakeADCSensor make_adc_sensor(const std::string &friendly_name,
                                uint8_t pin,
                                uint32_t update_interval = 15000);
#endif

#ifdef USE_ADS1115_SENSOR
  /** Create an ADS1115 component hub. From this hub you can then create individual sensors using `get_sensor()`.
   *
   * Note that you should have i2c setup for this component to work. To setup i2c call `App.init_i2c(SDA_PIN, SCL_PIN);`
   * before `App.setup()`.
   *
   * @param address The i2c address of the ADS1115. See ADS1115Component::set_address for possible values.
   * @return The ADS1115Component hub. Use this to set advanced setting and create the actual sensors.
   */
  sensor::ADS1115Component *make_ads1115_component(uint8_t address);
#endif

#ifdef USE_BMP085_SENSOR
  struct MakeBMP085Sensor {
    sensor::BMP085Component *bmp;
    sensor::MQTTSensorComponent *mqtt_temperature;
    sensor::MQTTSensorComponent *mqtt_pressure;
  };

  /** Create an BMP085/BMP180/BMP280 i2c temperature+pressure sensor.
   *
   * Be sure to initialize i2c before calling `App.setup()` in order for this to work. Do so
   * with `App.init_i2c(SDA_PIN, SCL_PIN);`.
   *
   * @param temperature_friendly_name The friendly name the temperature should be advertised as.
   * @param pressure_friendly_name The friendly name the pressure should be advertised as.
   * @param update_interval The interval in ms to update the sensor values.
   * @return A MakeBMP085Component object, use this to set advanced settings.
   */
  MakeBMP085Sensor make_bmp085_sensor(const std::string &temperature_friendly_name,
                                      const std::string &pressure_friendly_name,
                                      uint32_t update_interval = 30000);
#endif

#ifdef USE_HTU21D_SENSOR
  struct MakeHTU21DSensor {
    sensor::HTU21DComponent *htu21d;
    sensor::MQTTSensorComponent *mqtt_temperature;
    sensor::MQTTSensorComponent *mqtt_humidity;
  };

  /** Create a HTU21D i2c-based temperature+humidity highly accurate sensor.
   *
   * Be sure to initialize i2c before calling `App.setup` in order for this to work. Do so
   * with `App.init_i2c(SDA_PIN, SCL_PIN);`.
   *
   * @param temperature_friendly_name The friendly name the temperature sensor should be advertised as.
   * @param humidity_friendly_name The friendly name the humidity sensor should be advertised as.
   * @param update_interval The interval in ms to update the sensor values.
   * @return A MakeHTU21DComponent, use this to set advanced settings.
   */
  MakeHTU21DSensor make_htu21d_sensor(const std::string &temperature_friendly_name,
                                      const std::string &humidity_friendly_name,
                                      uint32_t update_interval = 15000);
#endif

#ifdef USE_HDC1080_SENSOR
  struct MakeHDC1080Sensor {
    sensor::HDC1080Component *hdc1080;
    sensor::MQTTSensorComponent *mqtt_temperature;
    sensor::MQTTSensorComponent *mqtt_humidity;
  };

  /** Create a HDC1080 i2c-based temperature+humidity sensor.
   *
   * Be sure to initialize i2c before calling `App.setup` in order for this to work. Do so
   * with `App.init_i2c(SDA_PIN, SCL_PIN);`.
   *
   * @param temperature_friendly_name The friendly name the temperature sensor should be advertised as.
   * @param humidity_friendly_name The friendly name the humidity sensor should be advertised as.
   * @param update_interval The interval in ms to update the sensor values.
   * @return A MakeHDC1080Sensor, use this to set advanced settings.
   */
  MakeHDC1080Sensor make_hdc1080_sensor(const std::string &temperature_friendly_name,
                                        const std::string &humidity_friendly_name,
                                        uint32_t update_interval = 15000);
#endif

#ifdef USE_ULTRASONIC_SENSOR
  struct MakeUltrasonicSensor {
    sensor::UltrasonicSensorComponent *ultrasonic;
    sensor::MQTTSensorComponent *mqtt;
  };

  /** Create an Ultrasonic range sensor.
   *
   * This can for example be an HC-SR04 ultrasonic sensor. It sends out a short ultrasonic wave and listens
   * for an echo. The time between the sending and receiving is then (with some maths) converted to a measurement
   * in meters. You need to specify the trigger pin (where to short pulse will be sent to) and the echo pin
   * (where we're waiting for the echo). Note that in order to not block indefinitely if we don't receive an
   * echo, this class has a default timeout of around 2m. You can change that using the return value of this
   * function.
   *
   * @param friendly_name The friendly name for this sensor advertised to Home Assistant.
   * @param trigger_pin The pin the short pulse will be sent to, can be integer or GPIOOutputPin.
   * @param echo_pin The pin we wait that we wait on for the echo, can be integer or GPIOInputPin.
   * @param update_interval The time in ms between updates, defaults to 5 seconds.
   * @return The Ultrasonic sensor + MQTT sensor pair, use this for advanced settings.
   */
  MakeUltrasonicSensor make_ultrasonic_sensor(const std::string &friendly_name,
                                              GPIOOutputPin trigger_pin, GPIOInputPin echo_pin,
                                              uint32_t update_interval = 5000);
#endif






  /*    ___  _   _ _____ ____  _   _ _____
   *   / _ \| | | |_   _|  _ \| | | |_   _|
   *  | | | | | | | | | | |_) | | | | | |
   *  | |_| | |_| | | | |  __/| |_| | | |
   *   \___/ \___/  |_| |_|    \___/  |_|
   */
#ifdef USE_OUTPUT
  /** Create a power supply component that will automatically switch on and off.
   *
   * @param pin The pin the power supply is connected to.
   * @param enable_time The time (in ms) the power supply needs until it can provide high power when powering on.
   * @param keep_on_time The time (in ms) the power supply should stay on when it is not used.
   * @return The PowerSupplyComponent.
   */
  PowerSupplyComponent *make_power_supply(GPIOOutputPin pin, uint32_t enable_time = 20,
                                          uint32_t keep_on_time = 10000);
#endif

#ifdef USE_LEDC_OUTPUT
  /** Create a ESP32 LEDC channel.
   *
   * @param pin The pin.
   * @param frequency The PWM frequency.
   * @param bit_depth The LEDC bit depth.
   * @return The LEDC component. Use this for advanced settings.
   */
  output::LEDCOutputComponent *make_ledc_output(uint8_t pin, float frequency = 1000.0f, uint8_t bit_depth = 12);
#endif

#ifdef USE_PCA9685_OUTPUT
  /** Create a PCA9685 component.
   *
   * @param frequency The PWM frequency.
   * @return The PCA9685 component. Use this for advanced settings.
   */
  output::PCA9685OutputComponent *make_pca9685_component(float frequency);
#endif

#ifdef USE_GPIO_OUTPUT
  /** Create a simple binary GPIO output component.
   *
   * Note: This is *only* a binary output component, not a switch that will be exposed
   * in Home Assistant. See make_simple_gpio_switch for a switch.
   *
   * @param pin The GPIO pin.
   * @return The GPIOBinaryOutputComponent. Use this for advanced settings.
   */
  output::GPIOBinaryOutputComponent *make_gpio_output(GPIOOutputPin pin);
#endif

#ifdef USE_ESP8266_PWM_OUTPUT
  /** Create an ESP8266 software PWM channel.
   *
   * Warning: This is a *software* PWM and therefore can have noticeable flickering. Additionally,
   * this software PWM can't output values higher than 80%.
   *
   * @param pin The pin for this PWM output, supported pins are 0-16.
   * @return The PWM output channel, use this for advanced settings and using it with lights.
   */
  output::ESP8266PWMOutput *make_esp8266_pwm_output(GPIOOutputPin pin);
#endif





  /*   _     ___ ____ _   _ _____
   *  | |   |_ _/ ___| | | |_   _|
   *  | |    | | |  _| |_| | | |
   *  | |___ | | |_| |  _  | | |
   *  |_____|___\____|_| |_| |_|
   */
#ifdef USE_LIGHT
  /// Create a MQTTJSONLightComponent. Mostly for internal use.
  light::MQTTJSONLightComponent *register_light(light::LightState *state);

  struct MakeLight {
    light::LinearLightOutputComponent *output;
    light::LightState *state;
    light::MQTTJSONLightComponent *mqtt;
  };

  /// Create and connect a MQTTJSONLightComponent to the provided light output component. Mostly for internal use.
  MakeLight connect_light_(const std::string &friendly_name, light::LinearLightOutputComponent *out);

  /** Create a binary light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param binary The binary output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  MakeLight make_binary_light(const std::string &friendly_name, output::BinaryOutput *binary);

  /** Create a monochromatic light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param mono The output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  MakeLight make_monochromatic_light(const std::string &friendly_name, output::FloatOutput *mono);

  /** Create a RGB light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param red The red output channel.
   * @param green The green output channel.
   * @param blue The blue output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  MakeLight make_rgb_light(const std::string &friendly_name,
                           output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue);

  /** Create a RGBW light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param red The red output channel.
   * @param green The green output channel.
   * @param blue The blue output channel.
   * @param white The white output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  MakeLight make_rgbw_light(const std::string &friendly_name,
                            output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue,
                            output::FloatOutput *white);
#endif





  /*   ______        _____ _____ ____ _   _
   *  / ___\ \      / |_ _|_   _/ ___| | | |
   *  \___ \\ \ /\ / / | |  | || |   | |_| |
   *   ___) |\ V  V /  | |  | || |___|  _  |
   *  |____/  \_/\_/  |___| |_| \____|_| |_|
   */
#ifdef USE_SWITCH
  /// Create a MQTTSwitchComponent for the provided Switch.
  switch_::MQTTSwitchComponent *register_switch(switch_::Switch *switch_);
#endif

#ifdef USE_IR_TRANSMITTER
  /** Create an IR transmitter.
   *
   * @param pin The pin the IR led is connected to.
   * @param carrier_duty_percent The duty cycle of the IR output. Decrease this if your LED gets hot.
   * @param clock_divider The clock divider for the rmt peripheral.
   * @return The IRTransmitterComponent. Use this for advanced settings.
   */
  switch_::IRTransmitterComponent *make_ir_transmitter(GPIOOutputPin pin,
                                                       uint8_t carrier_duty_percent = 50,
                                                       uint8_t clock_divider = switch_::DEFAULT_CLOCK_DIVIDER);
#endif

#ifdef USE_GPIO_SWITCH
  struct MakeGPIOSwitch {
    output::GPIOBinaryOutputComponent *gpio;
    switch_::SimpleSwitch *switch_;
    switch_::MQTTSwitchComponent *mqtt;
  };

  /** Create a simple GPIO switch that can be toggled on/off and appears in the Home Assistant frontend.
   *
   * @param pin The pin used for this switch. Can be integer or GPIOOutputPin.
   * @param friendly_name The friendly name advertised to Home Assistant for this switch-
   * @return A GPIOSwitchStruct, use this to set advanced settings.
   */
  MakeGPIOSwitch make_gpio_switch(const std::string &friendly_name, GPIOOutputPin pin);
#endif

#ifdef USE_RESTART_SWITCH
  struct MakeRestartSwitch {
    switch_::RestartSwitch *restart;
    switch_::MQTTSwitchComponent *mqtt;
  };

  /// Make a simple switch that restarts the device with the provided friendly name.
  MakeRestartSwitch make_restart_switch(const std::string &friendly_name);
#endif





  /*   _____ _    _   _
   *  |  ___/ \  | \ | |
   *  | |_ / _ \ |  \| |
   *  |  _/ ___ \| |\  |
   *  |_|/_/   \_|_| \_|
   */
#ifdef USE_FAN
  fan::MQTTFanComponent *register_fan(fan::FanState *state);

  struct MakeFan {
    fan::BasicFanComponent *output;
    fan::FanState *state;
    fan::MQTTFanComponent *mqtt;
  };

  /** Create and connect a Fan with the specified friendly name.
   *
   * @param friendly_name The friendly name of the Fan to advertise.
   * @return A FanStruct, use the output field to set your output channels.
   */
  MakeFan make_fan(const std::string &friendly_name);
#endif





  /*   _   _ _____ _     ____  _____ ____  ____
   *  | | | | ____| |   |  _ \| ____|  _ \/ ___|
   *  | |_| |  _| | |   | |_) |  _| | |_) \___ \
   *  |  _  | |___| |___|  __/| |___|  _ < ___) |
   *  |_| |_|_____|_____|_|   |_____|_| \_|____/
   */
#ifdef USE_DEBUG_COMPONENT
  DebugComponent *make_debug_component();
#endif

#ifdef USE_DEEP_SLEEP
  DeepSleepComponent *make_deep_sleep_component();
#endif

  /// Register the component in this Application instance.
  template<class C>
  C *register_component(C *c);

  template<class C>
  C *register_controller(C *c);

  /// Set up all the registered components. Call this at the end of your setup() function.
  void setup();

  /// Make a loop iteration. Call this in your loop() function.
  void loop();

  WiFiComponent *get_wifi() const;
  mqtt::MQTTClientComponent *get_mqtt_client() const;

  /// Get the name of this Application set by set_name().
  const std::string &get_name() const;

#ifdef USE_I2C
  void assert_i2c_initialized() const;
#endif

 protected:
  std::vector<Component *> components_{};
  std::vector<Controller *> controllers_{};
  mqtt::MQTTClientComponent *mqtt_client_{nullptr};
  WiFiComponent *wifi_{nullptr};

  std::string name_;
  Component::ComponentState application_state_{Component::CONSTRUCTION};
#ifdef USE_I2C
  bool i2c_initialized_{false};
#endif
};

/// Global storage of Application pointer - only one Application can exist.
extern Application App;

template<class C>
C *Application::register_component(C *c) {
  Component *component = c;
  if (c != nullptr)
    this->components_.push_back(component);
  return c;
}

template<class C>
C *Application::register_controller(C *c) {
  Controller *controller = c;
  this->controllers_.push_back(controller);
  return c;
}

} // namespace esphomelib

#endif //ESPHOMELIB_APPLICATION_H
