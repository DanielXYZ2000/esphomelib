//
// Created by Otto Winter on 03.12.17.
//

#include "esphomelib/ota_component.h"

#include <ArduinoOTA.h>

#include "esphomelib/log.h"
#include "esphomelib/esppreferences.h"
#include "esphomelib/application.h"

namespace esphomelib {

static const char *TAG = "ota";
static const char *PREF_TAG = "ota"; ///< Tag for preferences.
static const char *PREF_SAFE_MODE_COUNTER_KEY = "safe_mode";

void OTAComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up OTA...");
  ESP_LOGCONFIG(TAG, "    port: %u", this->port_);
  this->server_ = new WiFiServer(this->port_);
  this->server_->begin();

  if (!this->hostname_.empty()) {
    ESP_LOGCONFIG(TAG, "    hostname: '%s'", this->hostname_.c_str());
    ArduinoOTA.setHostname(this->hostname_.c_str());
  }
  ArduinoOTA.setPort(this->port_);
  switch (this->auth_type_) {
    case PLAINTEXT: {
      ArduinoOTA.setPassword(this->password_.c_str());
      break;
    }
    case HASH: {
      ArduinoOTA.setPasswordHash(this->password_hash_.c_str());
      break;
    }
    case OPEN: {}
  }

  ArduinoOTA.onStart([this]() {
    ESP_LOGI(TAG, "OTA starting...");
    this->ota_triggered_ = true;
  });
  ArduinoOTA.onEnd([&]() {
    ESP_LOGI(TAG, "OTA update finished!");
    ESP_LOGI(TAG, "Rebooting...");
    if (this->has_safe_mode_)
      // Don't make successful OTAs trigger boot loop detection.
      global_preferences.put_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, 0);
  });
  ArduinoOTA.onProgress([](uint progress, uint total) {
    float percentage = float(progress) * 100 / float(total);
    ESP_LOGD(TAG, "OTA in progress: %0.1f%%", percentage);
  });
  ArduinoOTA.onError([this](ota_error_t error) {
    ESP_LOGE(TAG, "Error[%u]: ", error);
    switch (error) {
      case OTA_AUTH_ERROR: {
        ESP_LOGE(TAG, "  Auth Failed");
        break;
      }
      case OTA_BEGIN_ERROR: {
        ESP_LOGE(TAG, "  Begin Failed");
        break;
      }
      case OTA_CONNECT_ERROR: {
        ESP_LOGE(TAG, "  Connect Failed");
        break;
      }
      case OTA_RECEIVE_ERROR: {
        ESP_LOGE(TAG, "  Receive Failed");
        break;
      }
      case OTA_END_ERROR: {
        ESP_LOGE(TAG, "  End Failed");
        break;
      }
      default:ESP_LOGE(TAG, "  Unknown Error");
    }
    this->ota_triggered_ = false;
  });
  ArduinoOTA.begin();
}

void OTAComponent::loop() {
  do {
    ArduinoOTA.handle();
    yield();
  } while (this->ota_triggered_);

  if (this->has_safe_mode_ && (millis() - this->safe_mode_start_time_) > this->safe_mode_enable_time_) {
    this->has_safe_mode_ = false;
    // successful boot, reset counter
    ESP_LOGI(TAG, "Boot seems successful, resetting boot loop counter.");
    global_preferences.put_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, 0);
  }
}

OTAComponent::OTAComponent(uint16_t port, std::string hostname)
    : port_(port), hostname_(std::move(hostname)), auth_type_(OPEN), server_(nullptr) {

}

void OTAComponent::set_auth_open() {
  this->auth_type_ = OPEN;
}
void OTAComponent::set_auth_plaintext_password(const std::string &password) {
  this->auth_type_ = PLAINTEXT;
  this->password_ = password;
}
void OTAComponent::set_auth_password_hash(const std::string &hash) {
  this->auth_type_ = HASH;
  this->password_hash_ = hash;
}
float OTAComponent::get_setup_priority() const {
  return setup_priority::MQTT_CLIENT;
}
uint16_t OTAComponent::get_port() const {
  return this->port_;
}
void OTAComponent::set_port(uint16_t port) {
  this->port_ = port;
}
const std::string &OTAComponent::get_hostname() const {
  return this->hostname_;
}
void OTAComponent::set_hostname(const std::string &hostname) {
  this->hostname_ = sanitize_hostname(hostname);
}
void OTAComponent::start_safe_mode(uint8_t num_attempts, uint32_t enable_time) {
  this->has_safe_mode_ = true;
  this->safe_mode_start_time_ = millis();
  this->safe_mode_enable_time_ = enable_time;

  uint8_t counter = global_preferences.get_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, 0);

  ESP_LOGCONFIG(TAG, "Safe mode enabled. There have been %u suspected unsuccessful boot attempts.", counter);

  if (counter >= num_attempts) {
    global_preferences.put_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, 0); // reset counter

    ESP_LOGE(TAG, "Boot loop detected. Proceeding to safe mode.");
    assert(App.get_wifi() != nullptr);

    App.get_wifi()->setup_();
    this->setup_();

    ESP_LOGI(TAG, "Waiting for OTA attempt.");
    uint32_t begin = millis();
    while ((millis() - begin) < enable_time) {
      this->loop_();
      App.get_wifi()->loop_();
    }
    ESP_LOGE(TAG, "No OTA attempt made, restarting.");
    ESP.restart();
  } else {
    // increment counter
    auto new_value = static_cast<uint8_t>(counter + 1);
    global_preferences.put_uint8(PREF_TAG, PREF_SAFE_MODE_COUNTER_KEY, new_value);
  }
}

} // namespace esphomelib
