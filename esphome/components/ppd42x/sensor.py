import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    CONF_ID,
    CONF_PM_10_0,
    CONF_PM_2_5,
    CONF_RX_ONLY,
    CONF_UPDATE_INTERVAL,
    DEVICE_CLASS_PM25,
    DEVICE_CLASS_PM10,
    STATE_CLASS_MEASUREMENT,
    UNIT_MICROGRAMS_PER_CUBIC_METER,
    ICON_CHEMICAL_WEAPON,
)

DEPENDENCIES = ["uart"]

ppd42x_ns = cg.esphome_ns.namespace("ppd42x")
PPD42XComponent = ppd42x_ns.class_("PPD42XComponent", uart.UARTDevice, cg.Component)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PPD42XComponent),
            cv.Optional(CONF_PM_2_5): sensor.sensor_schema(
                unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
                icon=ICON_CHEMICAL_WEAPON,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_PM25,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PM_10_0): sensor.sensor_schema(
                unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
                icon=ICON_CHEMICAL_WEAPON,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_PM10,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_RX_ONLY, default=False): cv.boolean,
            cv.Optional(CONF_UPDATE_INTERVAL): cv.positive_time_period_minutes,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_UPDATE_INTERVAL in config:
        cg.add(var.set_update_interval_min(config[CONF_UPDATE_INTERVAL]))
    cg.add(var.set_rx_mode_only(config[CONF_RX_ONLY]))

    if CONF_PM_2_5 in config:
        sens = await sensor.new_sensor(config[CONF_PM_2_5])
        cg.add(var.set_pm_2_5_sensor(sens))

    if CONF_PM_10_0 in config:
        sens = await sensor.new_sensor(config[CONF_PM_10_0])
        cg.add(var.set_pm_10_0_sensor(sens))
