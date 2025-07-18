#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define EXTPOWER_NODE DT_ALIAS(extpower)
#if !DT_NODE_HAS_STATUS(EXTPOWER_NODE, okay)
#error "No extpower alias found in devicetree"
#endif

static const struct gpio_dt_spec extpower_gpio = GPIO_DT_SPEC_GET(EXTPOWER_NODE, gpios);

int beh_extpower_toggle_on_hold_press(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    LOG_INF("EXTPOWER: pressed, disabling power");
    gpio_pin_set_dt(&extpower_gpio, 0); // 0 = off
    return ZMK_BEHAVIOR_TRANSPARENT;
}

int beh_extpower_toggle_on_hold_release(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    LOG_INF("EXTPOWER: released, enabling power");
    gpio_pin_set_dt(&extpower_gpio, 1); // 1 = on
    return ZMK_BEHAVIOR_TRANSPARENT;
}

static int beh_extpower_toggle_on_hold_init(void) {
    if (!device_is_ready(extpower_gpio.port)) {
        LOG_ERR("EXTPOWER GPIO device not ready");
        return -ENODEV;
    }
    gpio_pin_configure_dt(&extpower_gpio, GPIO_OUTPUT_INACTIVE); // 默认关闭
    return 0;
}

SYS_INIT(beh_extpower_toggle_on_hold_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

const struct behavior_driver_api beh_extpower_toggle_on_hold_driver_api = {
    .binding_pressed = beh_extpower_toggle_on_hold_press,
    .binding_released = beh_extpower_toggle_on_hold_release,
};