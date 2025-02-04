#include "ppd42x.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sensor {

static const char *TAG = "sensor.ppd42x";

PPD42XComponent::PPD42XComponent(PPD42XType type, uint32_t update_interval, uint32_t time_out)
    : ctype_(type), ui_(update_interval), timeout_ms_(time_out) {}

void PPD42XComponent::setup() {
  // because this implementation is currently rx-only, there is nothing to setup
}

void PPD42XComponent::loop() {
  const uint32_t now = millis();
  if ((now - this->starttime_) > this->ui_) {
    uint32_t duration_pl_02_5 = pulseIn(this->pl_02_5_sensor_->pl_pin_->get_pin(),
                                        uint8_t(!this->pl_02_5_sensor_->pl_pin_->is_inverted()), this->timeout_ms_);
    uint32_t duration_pl_10_0 = pulseIn(this->pl_10_0_sensor_->pl_pin_->get_pin(),
                                        uint8_t(!this->pl_10_0_sensor_->pl_pin_->is_inverted()), this->timeout_ms_);
    this->lowpulseoccupancy_02_5_ = this->lowpulseoccupancy_02_5_ + duration_pl_02_5;
    this->lowpulseoccupancy_10_0_ = this->lowpulseoccupancy_10_0_ + duration_pl_10_0;

    if ((now - this->starttime_) > this->timeout_ms_) {
      // last transmission too long ago
      this->starttime_ = now;
      parse_data_();
    }
  }
}

float PPD42XComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

void PPD42XComponent::parse_data_() {
  switch (this->ctype_) {
    case PPD42X_TYPE: {
      float pl_02_5_concentration = us_to_pl(this->lowpulseoccupancy_02_5_, this->timeout_ms_);
      float pl_10_0_concentration = us_to_pl(this->lowpulseoccupancy_10_0_, this->timeout_ms_);
      ESP_LOGD(TAG, "Got PM2.5 Concentration %.0f pcs/L, PM10.0 Concentration: %f pcs/L", pl_02_5_concentration,
               pl_10_0_concentration);
      if (this->pl_02_5_sensor_ != nullptr)
        this->pl_02_5_sensor_->publish_state(pl_02_5_concentration);
      if (this->pl_10_0_sensor_ != nullptr)
        this->pl_10_0_sensor_->publish_state(pl_10_0_concentration);
      break;
    }
    case PPD42X_TYPE_NS: {
      float pl_02_5_concentration = us_to_pl(this->lowpulseoccupancy_02_5_, this->timeout_ms_);
      float pl_10_0_concentration = us_to_pl(this->lowpulseoccupancy_10_0_, this->timeout_ms_);
      ESP_LOGD(TAG, "Got PM2.5 Concentration %.0f pcs/L, PM10.0 Concentration: %f pcs/L", pl_02_5_concentration,
               pl_10_0_concentration);
      if (this->pl_02_5_sensor_ != nullptr)
        this->pl_02_5_sensor_->publish_state(pl_02_5_concentration);
      if (this->pl_10_0_sensor_ != nullptr)
        this->pl_10_0_sensor_->publish_state(pl_10_0_concentration);
      break;
    }
    case PPD42X_TYPE_NJ: {
      float pl_02_5_concentration = us_to_pl(this->lowpulseoccupancy_02_5_, this->timeout_ms_);
      float pl_10_0_concentration = us_to_pl(this->lowpulseoccupancy_10_0_, this->timeout_ms_);
      ESP_LOGD(TAG, "Got PM2.5 Concentration %.0f pcs/L, PM10.0 Concentration: %f pcs/L", pl_02_5_concentration,
               pl_10_0_concentration);
      if (this->pl_02_5_sensor_ != nullptr)
        this->pl_02_5_sensor_->publish_state(pl_02_5_concentration);
      if (this->pl_10_0_sensor_ != nullptr)
        this->pl_10_0_sensor_->publish_state(pl_10_0_concentration);
      break;
    }
  }
}

float PPD42XComponent::us_to_pl(uint32_t sample_length, uint32_t time_pm) {
  float ratio = time_pm / (sample_length * 10.0f);
  return 1.1f * powf(ratio, 3) - 3.8f * powf(ratio, 2) + 520.0f * ratio + 0.62f;
}

PPD42XSensor *PPD42XComponent::make_pl_02_5_sensor(const std::string &name, GPIOInputPin *pl) {
  return this->pl_02_5_sensor_ = new PPD42XSensor(name, pl, PPD42X_SENSOR_TYPE_PM_02_5);
}

PPD42XSensor *PPD42XComponent::make_pl_10_0_sensor(const std::string &name, GPIOInputPin *pl) {
  return this->pl_10_0_sensor_ = new PPD42XSensor(name, pl, PPD42X_SENSOR_TYPE_PM_10_0);
}

void PPD42XComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "PPD42X:");
  LOG_SENSOR("  ", "PM2.5", this->pl_02_5_sensor_);
  LOG_SENSOR("  ", "PM10.0", this->pl_10_0_sensor_);
}

std::string PPD42XSensor::unit_of_measurement() {
  switch (this->stype_) {
    case PPD42X_SENSOR_TYPE_PM_02_5:
    case PPD42X_SENSOR_TYPE_PM_10_0:
      return UNIT_PARTICLES_PER_LITER;
  }
  return "";
}

std::string PPD42XSensor::icon() {
  switch (this->stype_) {
    case PPD42X_SENSOR_TYPE_PM_02_5:
    case PPD42X_SENSOR_TYPE_PM_10_0:
      // Not the ideal icon, but Otto can't find a better one ;)
      return ICON_CHEMICAL_WEAPON;
  }
  return "";
}

int8_t PPD42XSensor::accuracy_decimals() {
  switch (this->stype_) {
    case PPD42X_SENSOR_TYPE_PM_02_5:
    case PPD42X_SENSOR_TYPE_PM_10_0:
      return 0;
  }
  return 0;
}
// void PPD42XComponent::set_timeout_us(uint32_t timeout_us) { this->timeout_ms_ = timeout_us; }

PPD42XSensor::PPD42XSensor(const std::string &name, GPIOInputPin *pl, PPD42XSensorType type)
    : Sensor(name), pl_pin_(pl), stype_(type) {}
}  // namespace sensor
}
