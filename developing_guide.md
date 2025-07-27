# Building ZMK Behavior Extensions: A Complete Developer Guide

*Learn how to extend ZMK firmware with custom behaviors like LED control, power management, or novel input features using the GPIO hold behavior as a real-world template.*

---

## Table of Contents
1. [Getting Started](#getting-started)
2. [Architecture Overview](#architecture-overview)
3. [Step-by-Step Implementation](#step-by-step-implementation)
4. [File Structure Deep Dive](#file-structure-deep-dive)
5. [Real-World Example: GPIO Hold Analysis](#real-world-example)
6. [Testing & Debugging](#testing--debugging)
7. [Publishing Your Extension](#publishing-your-extension)
8. [Advanced Patterns](#advanced-patterns)

---

## Getting Started

### What is a ZMK Behavior Extension?

ZMK behavior extensions are Zephyr modules that add new keyboard behaviors like key functions, LED control, power management, or specialized input handling. They integrate seamlessly with ZMK's keymap system and device tree configuration.

### Prerequisites

- **Knowledge**: Basic C, Zephyr RTOS concepts, device tree
- **Tools**: `west` build system, Zephyr SDK, Git
- **Hardware**: ZMK-compatible keyboard for testing

---

## Architecture Overview

### The Behavior Lifecycle

```
Device Tree → Kernel Init → Keymap Binding → Behavior Callback → Hardware Control
```

1. **Device Tree**: Static configuration (GPIO pins, I2C addresses, etc.)
2. **Kernel Init**: Zephyr device driver initialization hook
3. **Keymap Binding**: Runtime mapping from key events to behavior instances
4. **Behavior Callback**: Your custom logic for press/release events
5. **Hardware Control**: Actual I/O operations

### Core Components

- **Behavior Driver**: The C implementation file
- **Device Tree Binding**: YAML describing configuration format
- **CMake Integration**: Build system integration
- **Kconfig Options**: Feature toggles and configuration
- **Module Metadata**: Zephyr module identification

---

## Step-by-Step Implementation

### Step 1: Project Scaffolding

Create your project structure:

```bash
mkdir zmk-behavior-myext
cd zmk-behavior-myext
git init
````

Create the directory structure:
```
zmk-behavior-myext/
├── src/
│   └── behaviors/
│       └── behavior_myext.c
├── dts/
│   └── bindings/
│       └── behaviors/
│           └── zmk,behavior-myext.yaml
├── include/
│   └── myext.h
├── CMakeLists.txt
├── Kconfig
├── zephyr/
│   └── module.yml
└── README.md
```

### Step 2: Create the Behavior Driver

**Basic template structure:**

```c
// src/behaviors/behavior_myext.c
#define DT_DRV_COMPAT zmk_behavior_myext

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zmk/behavior.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Configuration
struct behavior_myext_config {
    // Add your configuration fields
};

// Runtime data
struct behavior_myext_data {
    // Add runtime state
};

// Implementation goes here...
```

### Step 3: Device Tree Configuration

**Create binding in `dts/bindings/behaviors/zmk,behavior-myext.yaml`:**

```yaml
# Copyright (c) 2024, Your Name
# SPDX-License-Identifier: MIT

description: |
  Your behavior description here

compatible: "zmk,behavior-myext"

# For no-parameter behaviors:
include: zero_param.yaml

# For parameter behaviors, use:
# include: one_param.yaml

properties:
  control-gpios:
    type: phandle-array
    description: GPIO pins controlled by this behavior
    required: true
  
  optional-flag:
    type: boolean
    description: Optional boolean configuration
```

---

## File Structure Deep Dive

### 1. Implementation File (`behavior_*.c`)

The heart of your extension lives here:

#### Configuration Struct
```c
struct behavior_gpio_hold_config {
    const struct gpio_dt_spec *gpios;
    size_t gpio_count;
};
```

#### Initialization Function
```c
static int behavior_myext_init(const struct device *dev) {
    const struct behavior_myext_config *config = dev->config;
    
    // Hardware initialization
    // Error checking
    // Logging
    
    return 0; // Success
}
```

#### Behavior Callbacks
```c
static int on_pressed(struct zmk_behavior_binding *binding,
                    struct zmk_behavior_binding_event event) {
    // Handle key press
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_released(struct zmk_behavior_binding *binding,
                    struct zmk_behavior_binding_event event) {
    // Handle key release
    return ZMK_BEHAVIOR_OPAQUE;
}
```

#### Driver Registration
```c
static const struct behavior_driver_api behavior_myext_api = {
    .binding_pressed = on_pressed,
    .binding_released = on_released,
};

// Instantiate for each device tree node
#define MYEXT_INST(n) \
    // ... macro for instance generation

DT_INST_FOREACH_STATUS_OKAY(MYEXT_INST)
```

### 2. Build System Integration

#### CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.20.0)

if(CONFIG_ZMK_BEHAVIOR_MYEXT)
  target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_MYEXT 
                      app PRIVATE src/behaviors/behavior_myext.c)
endif()
```

#### Kconfig
```kconfig
config ZMK_BEHAVIOR_MYEXT
    bool "My Extension Behavior"
    default n  # Set to y for auto-enablement
    
if ZMK_BEHAVIOR_MYEXT

config ZMK_BEHAVIOR_MYEXT_LOG_LEVEL
    int "Log level for my behavior"
    default 0
    range 0 4
    help
      0: OFF, 1: ERR, 2: WRN, 3: INF, 4: DBG

endif # ZMK_BEHAVIOR_MYEXT
```

---

## Real-World Example: GPIO Hold Analysis

Let's examine the actual GPIO Hold behavior to understand production patterns:

### GPIO Pin Handling

```c
// From behavior_gpio_hold.c - GPIO management
static const struct gpio_dt_spec gpio_hold_gpios_0[] = {
    DT_INST_FOREACH_PROP_ELEM_SEP(0, control_gpios, GPIO_DT_SPEC_GET_BY_IDX, (,)
};

static int behavior_gpio_hold_init(const struct device *dev) {
    const struct behavior_gpio_hold_config *config = dev->config;
    
    for (size_t i = 0; i < config->gpio_count; i++) {
        if (!gpio_is_ready_dt(&config->gpios[i])) {
            LOG_ERR("GPIO device %s not ready", 
                   config->gpios[i].port->name);
            return -ENODEV;
        }
        
        int ret = gpio_pin_configure_dt(&config->gpios[i], 
                                      GPIO_OUTPUT_INACTIVE);
        if (ret < 0) {
            LOG_ERR("Failed to configure GPIO pin %d: %d", 
                   config->gpios[i].pin, ret);
            return ret;
        }
    }
}
```

### Multi-Instance Pattern

```c
// Automatic multi-instance support
#define GPIO_HOLD_INST(n) \                                                      \
    static const struct gpio_dt_spec gpio_hold_gpios_##n[] = \                 \
        { DT_INST_FOREACH_PROP_ELEM_SEP(n, control_gpios, \                   \
                                       GPIO_DT_SPEC_GET_BY_IDX, (,)) }; \     \
    static const struct behavior_gpio_hold_config \                           \
        gpio_hold_config_##n = { \                                          \
        .gpios = gpio_hold_gpios_##n, \                                     \
        .gpio_count = ARRAY_SIZE(gpio_hold_gpios_##n), \                    \
    }; \                                                                      \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_gpio_hold_init, NULL, \              \
                           NULL, &gpio_hold_config_##n, \                   \
                           POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, \
                           &behavior_gpio_hold_driver_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_HOLD_INST)
```

---

## Testing & Debugging

### Development Commands

```bash
# Initialize development environment
cd tests && ./build.sh

# Build for specific board
west build -s zmk/app -d .build/test -b nice_nano_v2 \
  -- -DZMK_CONFIG=$(pwd) -DZMK_EXTRA_MODULES=$(pwd)/..

# Clean build
west build -t clean

# Enable debug logging
west build ... -- -DCONFIG_ZMK_BEHAVIOR_GPIO_HOLD_LOG_LEVEL=4
```

### Debugging Tips

1. **Device Tree Verification**:
   ```bash
   # Check device tree compilation
   west build -t dt_struct
   
   # List available devices
   west build -t dt_supported
   ```

2. **Runtime Debugging**:
   - Use `LOG_DBG()`, `LOG_INF()`, `LOG_ERR()` macros
   - Monitor UART output for initialization messages
   - Verify GPIO configuration with `gpiod get` on Linux (if available)

3. **Common Issues**:
   - **GPIO not found**: Check device tree node names and paths
   - **Permission errors**: Verify GPIO driver support in Zephyr
   - **Build failures**: Ensure all paths are absolute in west commands

---

## Publishing Your Extension

### 1. Repository Structure

Ensure your repo is clean and well-documented:

```
README.md                # Comprehensive documentation
CONTRIBUTING.md          # Development guidelines
examples/                # Usage examples
├── basic/keymap.dtsi
├── advanced/keymap.dtsi
tests/                  # Test configurations
├── native_posix.conf
├── board-specific/
```

### 2. Package Management

#### west.yml Integration

Users add your module to their project:

```yaml
# In user's west.yml
manifest:
  projects:
    - name: zmk-behavior-myext
      url: https://github.com/yourusername/zmk-behavior-myext
      revision: main
      path: modules/lib/zmk-behavior-myext
```

### 3. Distribution README

Create comprehensive documentation:

```markdown
# My ZMK Behavior Extension

## Installation
1. Add to your `west.yml` ...
2. Enable in your `config.conf` ...
3. Add to keymap ...

## Configuration
```dts
// Add configuration examples
```

## Usage Examples
```dts
// Real-world keymap snippets
```
```

---

## Advanced Patterns

### Parameterized Behaviors

For behaviors that accept parameters:

```c
// Binding gets parameter
static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                   struct zmk_behavior_binding_event event) {
    uint32_t param = binding->param1;
    // Use param for behavior selection
}
```

```yaml
# Device tree binding for parameterized behavior
description: Parameterized behavior
custom-params:
  params-flag:
    type: int
    required: true
    description: Behavior parameter value
```

### Complex State Management

```c
// Runtime data with state tracking
struct behavior_myext_data {
    uint8_t current_channel;
    bool state[4];
    k_timeout_t last_change;
};

// Periodic updates
static void myext_timer_handler(struct k_timer *timer) {
    struct behavior_myext_data *data = 
        CONTAINER_OF(timer, struct behavior_myext_data, timer);
    // Handle periodic tasks
}
```

### Hardware Abstraction

```c
// Platform-specific implementations
#ifdef CONFIG_BOARD_NICE_NANO_V2
    #define PLATFORM_MAX_CHANNELS 8
#else
    #define PLATFORM_MAX_CHANNELS 4
#endif
```

---

## Summary

Developing ZMK behavior extensions follows a consistent pattern across different hardware integrations. The GPIO Hold behavior demonstrates production-grade patterns that you can adapt for:

- **LED controllers** (WS2812B, APA102)
- **Power management** (battery monitoring, sleep/wake)
- **Input devices** (encoders, trackballs, joysticks)
- **Communication** (BLE bonding, wireless pairing)
- **Specialized behaviors** (macro triggering, layer management)

The key is following Zephyr's device driver patterns while integrating cleanly with ZMK's keymap system. Start with the GPIO Hold template and adapt the specific hardware interaction patterns you need.

## Next Steps

1. **Fork the GPIO Hold repo** as a starting template
2. **Identify your hardware interface** (GPIO, I2C, SPI, etc.)
3. **Modify specific callbacks** for your use case
4. **Test on real hardware** following the debugging guide
5. **Share your extension** with the ZMK community

Happy building! 🚀

---

*This guide is based on the ZMK External Power Hold behavior [github link](https://github.com/hitsmaxft/zmk-behavior-gpio-hold). 
