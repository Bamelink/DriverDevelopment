#!/bin/bash

source ../zephyr/zephyr-env.sh
west build -b nucleo_l073rz -p always -- -DDTC_OVERLAY_FILE="l073rz.overlay"