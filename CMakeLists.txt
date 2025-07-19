# Copyright (c) 2025, The ZMK Contributors
# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_BEHAVIOR_GPIO_HOLD)
  # zephyr_library()
  # zephyr_library_include_directories(include)
  
  # Include ZMK headers from the app
  target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_GPIO_HOLD app PRIVATE src/behaviors/behavior_gpio_hold.c)
endif()