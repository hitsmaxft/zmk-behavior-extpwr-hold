
# ZMK Multi-Channel GPIO Control behavior Module

This module provides gpio control capabilities for ZMK firmware, supporting multiple independent power channels with press-on/release-off functionality.

## Features

- **Independent control**: Each channel can be controlled separately via key bindings
- **Press-on/release-off**: New behavior that turns gpio ON when key is pressed and OFF when released
- **Backward compatibility**: Original toggle behavior is preserved
- **Flexible configuration**: Configure any number of channels based on your hardware setup

## Technical Details

- **GPIO Logic**: High level (1) = power ON, Low level (0) = power OFF
- **Initialization**: All channels are initialized to OFF state
- **Multi-board support**: Automatically detects available gpio aliases
- **Error handling**：Gracefully handles missing GPIO configurations

## Configuration Options

- `ZMK_BEHAVIOR_GPIO_HOLD=y`: Enable the new press-on/release-off behavior (default)

## Examples

### Basic Setup with 2 Channels
```dts
/ {
    behaviors {
        ep_hold: behavior_gpio_hold {
            compatible = "zmk,behavior-gpio-hold";
            #binding-cells = <0>;
            control-gpios = <&gpio0 13 GPIO_ACTIVE_HIGH 
                        &gpio0 14 GPIO_ACTIVE_HIGH> ;
        };
    };
    keymap {
        compatible = "zmk,keymap";
        default_layer {

            bindings =<
                    &kp ESC   &kp N1         &kp N2        &kp N3        &kp N4               &kp N5                                        &kp N6     &kp N7               &kp N8        &kp N9        &kp N0            &kp BSPC
                    &ep_hold
            >;
        };
    };

};
```
