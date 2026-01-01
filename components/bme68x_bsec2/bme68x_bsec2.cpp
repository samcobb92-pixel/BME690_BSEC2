#include "bme68x_bsec2.h"
#include "esphome/core/log.h"

namespace esphome {
namespace bme68x_bsec2 {

static const char *TAG = "bme690";

void BME68xBSEC2Component::set_model(const std::string &model) {
  if (model == "BME690") model_ = BMEModel::BME690;
  else if (model == "BME688") model_ = BMEModel::BME688;
  else model_ = BMEModel::BME680;
}

void BME68xBSEC2Component::setup() {
  dev_.intf = BME69X_I2C_INTF;
  dev_.read = read_;
  dev_.write = write_;
  dev_.delay_us = delay_us_;
  dev_.intf_ptr = this;

  ESP_LOGI(TAG, "Initializing BME sensor");
  bme69x_init(&dev_);

  ESP_LOGI(TAG, "Initializing BSEC2");
  bsec_init();
}

void BME68xBSEC2Component::update() {
  struct bme69x_data data;
  uint8_t n_fields;

  if (bme69x_get_data(BME69X_FORCED_MODE, &data, &n_fields, &dev_) != BME69X_OK)
    return;

  if (temperature_) temperature_->publish_state(data.temperature);
  if (humidity_) humidity_->publish_state(data.humidity);
  if (pressure_) pressure_->publish_state(data.pressure / 100.0f);
  if (gas_) gas_->publish_state(data.gas_resistance);

  bsec_input_t inputs[3];
  uint8_t num_inputs = 0;
  uint64_t ts = millis() * 1000ULL;

  inputs[num_inputs++] = {BSEC_INPUT_TEMPERATURE, data.temperature, ts};
  inputs[num_inputs++] = {BSEC_INPUT_HUMIDITY, data.humidity, ts};
  inputs[num_inputs++] = {BSEC_INPUT_GASRESISTOR, data.gas_resistance, ts};

  bsec_output_t outputs[BSEC_NUMBER_OUTPUTS];
  uint8_t num_outputs;

  bsec_do_steps(inputs, num_inputs, outputs, &num_outputs);

  for (uint8_t i = 0; i < num_outputs; i++) {
    if (outputs[i].sensor_id == BSEC_OUTPUT_IAQ && iaq_)
      iaq_->publish_state(outputs[i].signal);
    else if (outputs[i].sensor_id == BSEC_OUTPUT_CO2_EQUIVALENT && co2_)
      co2_->publish_state(outputs[i].signal);
    else if (outputs[i].sensor_id == BSEC_OUTPUT_BREATH_VOC_EQUIVALENT && voc_)
      voc_->publish_state(outputs[i].signal);
  }
}

int8_t BME68xBSEC2Component::read_(uint8_t reg, uint8_t *data,
                                  uint32_t len, void *ptr) {
  auto *self = static_cast<BME68xBSEC2Component *>(ptr);
  return self->read_bytes(reg, data, len) ? 0 : -1;
}

int8_t BME68xBSEC2Component::write_(uint8_t reg, const uint8_t *data,
                                   uint32_t len, void *ptr) {
  auto *self = static_cast<BME68xBSEC2Component *>(ptr);
  return self->write_bytes(reg, data, len) ? 0 : -1;
}

void BME68xBSEC2Component::delay_us_(uint32_t period, void *) {
  delayMicroseconds(period);
}

}  // namespace bme68x_bsec2
}  // namespace esphome