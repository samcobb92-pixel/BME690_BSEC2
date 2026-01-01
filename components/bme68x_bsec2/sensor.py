import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import *

DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@yourgithub"]

bme_ns = cg.esphome_ns.namespace("bme68x_bsec2")
BMEComponent = bme_ns.class_(
    "BME68xBSEC2Component", cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BMEComponent),

    cv.Required("model"): cv.one_of("BME680", "BME688", "BME690"),

    cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=2,
    ),
    cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=2,
    ),
    cv.Optional(CONF_PRESSURE): sensor.sensor_schema(
        unit_of_measurement=UNIT_HECTOPASCAL,
        accuracy_decimals=2,
    ),
    cv.Optional("gas_resistance"): sensor.sensor_schema(
        unit_of_measurement=UNIT_OHM,
        accuracy_decimals=0,
    ),

    cv.Optional("iaq"): sensor.sensor_schema(accuracy_decimals=0),
    cv.Optional("co2_equivalent"): sensor.sensor_schema(
        unit_of_measurement=UNIT_PARTS_PER_MILLION,
        accuracy_decimals=0,
    ),
    cv.Optional("breath_voc_equivalent"): sensor.sensor_schema(
        unit_of_measurement=UNIT_PARTS_PER_MILLION,
        accuracy_decimals=2,
    ),
}).extend(i2c.i2c_device_schema(0x77))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_model(config["model"]))

    for key in [
        CONF_TEMPERATURE,
        CONF_HUMIDITY,
        CONF_PRESSURE,
        "gas_resistance",
        "iaq",
        "co2_equivalent",
        "breath_voc_equivalent",
    ]:
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(var, f"set_{key}_sensor")(sens))