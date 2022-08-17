# SPDX-License-Identifier: Apache-2.0

if(CONFIG_LEDCONTROLLER)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    ledcontroller.c
    )
endif()
