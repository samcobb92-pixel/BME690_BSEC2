#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

extern "C" {
#include "bme69x.h"
#include "bsec_interface.h"
}

namespace esphome {
namespace bme68x_bsec2 {

enum class BMEModel { BME680, BME688, BME690 };

class BME68xBSEC2Component : public PollingComponent,
                            public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;

  void set_model(const std::string &model);

  void set_temperature_sensor(sensor::Sensor *s) { temperature_ = s; }
  void set_humidity_sensor(sensor::Sensor *s) { humidity_ = s; }
  void set_pressure_sensor(sensor::Sensor *s) { pressure_ = s; }
  void set_gas_resistance_sensor(sensor::Sensor *s) { gas_ = s; }

  void set_iaq_sensor(sensor::Sensor *s) { iaq_ = s; }
  void set_co2_equivalent_sensor(sensor::Sensor *s) { co2_ = s; }
  void set_breath_voc_equivalent_sensor(sensor::Sensor *s) { voc_ = s; }

 protected:
  BMEModel model_;
  struct bme69x_dev dev_;

  sensor::Sensor *temperature_{nullptr};
  sensor::Sensor *humidity_{nullptr};
  sensor::Sensor *pressure_{nullptr};
  sensor::Sensor *gas_{nullptr};
  sensor::Sensor *iaq_{nullptr};
  sensor::Sensor *co2_{nullptr};
  sensor::Sensor *voc_{nullptr};

  static int8_t read_(uint8_t, uint8_t *, uint32_t, void *);
  static int8_t write_(uint8_t, const uint8_t *, uint32_t, void *);
  static void delay_us_(uint32_t, void *);
};

}  // namespace bme68x_bsec2
}  // namespace esphome