import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_ID, CONF_MIN_FREQ_MHZ, CONF_MAX_FREQ_MHZ

pm_ns = cg.esphome_ns.namespace('pm')
PM = pm_ns.class_('PM', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PM),
    cv.Optional(CONF_MIN_FREQ_MHZ, default=40): cv.uint16_t,
    cv.Optional(CONF_MAX_FREQ_MHZ, default=240): cv.uint16_t,
}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    cg.add(var.set_freq(config[CONF_MIN_FREQ_MHZ], config[CONF_MAX_FREQ_MHZ]))

    yield cg.register_component(var, config)

    cg.add_define('USE_PM')
