# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview
ZMK External Power Hold module is a Zephyr RTOS module that provides press-to-enable, release-to-disable GPIO control for gpios switching circuits in mechanical keyboards. This is particularly useful for controlling external devices like LED strips, displays, or other peripherals.

## Architecture
- **Module**: ZMK (Zephyr Mechanical Keyboard) behavior extension
- **Communication**: Zephyr GPIO drivers with device tree configuration
- **Event Handling**: ZMK behavior system callbacks (press/release)

## Key Files
- `src/behaviors/behavior_gpio_hold.c` - Main behavior implementation
- `src/gpio_hold.c` - Alternative implementation with legacy support
- `include/gpio_hold_on.h` - Public API headers
- `dts/bindings/behaviors/zmk,behavior-gpio-hold.yaml` - Device tree binding
- `Kconfig` - Zephyr kernel configuration options

## Build System
- Uses **west** with **CMake** for building
- Requires ZMK source as dependency
- Managed via west.yml in config directory

## Development Commands

### Build Setup
Initialize west workspace:
```bash
./build.sh
```

### Build for Testing
```bash
west build -s zmk/app -d $(pwd)/.build/sofle_left -b nice_nano_v2 \
     -- -DZMK_CONFIG=$(pwd)/config -DZMK_EXTRA_MODULES=$(pwd) -DSHIELD=sofle_left | grep error 
```

### Nix Environment (Optional)
```bash
nix develop
```

### Configuration
Enable module in device config:
```bash
echo "CONFIG_ZMK_BEHAVIOR_GPIO_HOLD=y" >> path/to/config/keyboard.conf
```

## Usage in Keymap

```dts
/ {
    behaviors {
        eph: behavior_gpio_hold {
            compatible = "zmk,behavior-gpio-hold";
            label = "GPIO_HOLD";
            #binding-cells = <0>;
            control-gpios = <&gpio0 13 GPIO_ACTIVE_HIGH 
                        &gpio0 14 GPIO_ACTIVE_HIGH> ;
        };
    };

    keymap {
        bindings = <&eph>;  // Channel 1 control
    };
};
```

## Key Parameters
- **GPIO flags**: Configurable active high/low
- **Power-on**: On key press
- **Power-off**: On key release

## Testing
The module includes:
- Multi gpio support
- GPIO initialization at boot
- Runtime grained control
- Logging at configurable levels
- Error handling for missing hardware
