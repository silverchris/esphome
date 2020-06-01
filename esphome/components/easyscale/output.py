from esphome import pins, automation
from esphome.components import output
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_PIN, ESP_PLATFORM_ESP32

ESP_PLATFORMS = [ESP_PLATFORM_ESP32]

easyscale_ns = cg.esphome_ns.namespace('easyscale')
EasyScale = easyscale_ns.class_('EasyScale', output.FloatOutput, cg.Component)

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend({
    cv.Required(CONF_ID): cv.declare_id(EasyScale),
    cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    gpio = yield cg.gpio_pin_expression(config[CONF_PIN])
    var = cg.new_Pvariable(config[CONF_ID], gpio)
    yield cg.register_component(var, config)
    yield output.register_output(var, config)
