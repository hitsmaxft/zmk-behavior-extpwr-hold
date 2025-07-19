/**
 * @file behavior_gpio_hold.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-07-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#define DT_DRV_COMPAT zmk_behavior_gpio_hold


#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zmk/behavior.h>
#include <drivers/behavior.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct behavior_gpio_hold_config {
    const struct gpio_dt_spec *gpios;
    size_t gpio_count;
};

struct behavior_gpio_hold_data {
    // No runtime data needed for this simple behavior
};

static int behavior_gpio_hold_init(const struct device *dev) {
    const struct behavior_gpio_hold_config *config = dev->config;
    
    for (size_t i = 0; i < config->gpio_count; i++) {
        if (!gpio_is_ready_dt(&config->gpios[i])) {
            LOG_ERR("GPIO device %s not ready", config->gpios[i].port->name);
            return -ENODEV;
        }
        
        int ret = gpio_pin_configure_dt(&config->gpios[i], GPIO_OUTPUT_INACTIVE);
        if (ret < 0) {
            LOG_ERR("Failed to configure GPIO pin %d: %d", config->gpios[i].pin, ret);
            return ret;
        }
    }
    
    LOG_DBG("Initialized gpio hold behavior with %zu GPIO pins", config->gpio_count);
    return 0;
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                   struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_gpio_hold_config *config = dev->config;
    
    for (size_t i = 0; i < config->gpio_count; i++) {
        gpio_pin_set_dt(&config->gpios[i], 1);
    }
    
    LOG_DBG("GPIO turned ON");
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                    struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_gpio_hold_config *config = dev->config;
    
    for (size_t i = 0; i < config->gpio_count; i++) {
        gpio_pin_set_dt(&config->gpios[i], 0);
    }
    
    LOG_DBG("GPIO turned OFF");
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_gpio_hold_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

#define GPIO_HOLD_INST(n)                                                           \
    static const struct gpio_dt_spec gpio_hold_gpios_##n[] =                      \
        { COND_CODE_1(DT_INST_NODE_HAS_PROP(n, control_gpios),                       \
                     (DT_INST_FOREACH_PROP_ELEM_SEP(n, control_gpios,               \
                                                    GPIO_DT_SPEC_GET_BY_IDX, (,))), \
                     ()) };                                                          \
    static const struct behavior_gpio_hold_config gpio_hold_config_##n = {    \
        .gpios = gpio_hold_gpios_##n,                                             \
        .gpio_count = ARRAY_SIZE(gpio_hold_gpios_##n),                           \
    };                                                                                 \
    static struct behavior_gpio_hold_data gpio_hold_data_##n = {};           \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_gpio_hold_init, NULL,                    \
                           &gpio_hold_data_##n, &gpio_hold_config_##n,       \
                           POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,         \
                           &behavior_gpio_hold_driver_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_HOLD_INST)
