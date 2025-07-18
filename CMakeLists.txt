if(CONFIG_ZMK_EXTPOWER_HOLD)
  zephyr_library()
  zephyr_library_sources( src/extpower_hold.c)
endif()
