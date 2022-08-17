# SPDX-License-Identifier: Apache-2.0

if(CONFIG_STEPPERCONTROLLER)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    steppercontroller.c
    )
endif()
