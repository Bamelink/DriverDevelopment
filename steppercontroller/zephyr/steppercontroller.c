/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT jan_steppercontroller

#include "steppercontroller.h"
#include <zephyr/types.h>
#include <zephyr/syscall_handler.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(steppercontroller, CONFIG_GPIO_LOG_LEVEL);

struct steppercontroller_config {
	struct gpio_dt_spec aled;
	struct gpio_dt_spec bled;
	struct gpio_dt_spec cled;
	struct gpio_dt_spec dled;
};

struct steppercontroller_data {
	STEPS last_state;
};

static void make_step(const struct steppercontroller_config *config, STEPS step) {
	gpio_pin_set_dt(&config->aled, (step & 0b0001));
	gpio_pin_set_dt(&config->bled, (step & 0b0010));
	gpio_pin_set_dt(&config->cled, (step & 0b0100));
	gpio_pin_set_dt(&config->dled, (step & 0b1000));
}

static void step_impl(const struct device *dev, DIRECTION dir, uint32_t steps)
{
	const struct steppercontroller_config *config = dev->config;
	struct steppercontroller_data *data = dev->data;

	if(CLOCKWISE) {
		for (int i=0; i<steps; i++) {
			if(data->last_state == FIRST) {
				data->last_state = SECOND;
			} else if(data->last_state == SECOND) {
				data->last_state = THIRD;
			} else if(data->last_state == THIRD) {
				data->last_state = FOURTH;
			} else if(data->last_state == FOURTH) {
				data->last_state = FIRST;
			}
			make_step(config, data->last_state);
			k_msleep(200);
		}
	} else if (COUNTER_CLOCKWISE) {
		for (int i=0; i<steps; i++) {
			if(data->last_state == FIRST) {
				data->last_state = FOURTH;
			} else if(data->last_state == SECOND) {
				data->last_state = FIRST;
			} else if(data->last_state == THIRD) {
				data->last_state = SECOND;
			} else if(data->last_state == FOURTH) {
				data->last_state = THIRD;
			}
			make_step(config, data->last_state);
			k_msleep(2);
		}
	}
}

#ifdef CONFIG_USERSPACE
static inline void z_vrfy_steppercontroller_step(const struct device *dev, uint8_t decimal)
{
	Z_OOPS(Z_SYSCALL_DRIVER_STEPPERCONTROLLER(dev, step));

	z_impl_steppercontroller_step(dev, decimal);
}
#include <syscalls/steppercontroller_show_binary_mrsh.c>
#endif /* CONFIG_USERSPACE */


static int steppercontroller_init(const struct device *dev)
{
	const struct steppercontroller_config *config = dev->config;
	struct steppercontroller_data *data = dev->data;
	data->last_state = FIRST;

	int ret;
	if (!device_is_ready(config->aled.port) && !device_is_ready(config->bled.port) &&\
	    !device_is_ready(config->cled.port) && !device_is_ready(config->dled.port)) {
		LOG_ERR("GPIOs not ready");
		return -ENODEV;
	}
	ret = gpio_pin_configure_dt(&config->aled, GPIO_OUTPUT);
	if (ret < 0) {
		LOG_ERR("Could not configure A GPIO (%d)", ret);
		return ret;
	}
	ret = gpio_pin_configure_dt(&config->bled, GPIO_OUTPUT);
	if (ret < 0) {
		LOG_ERR("Could not configure B GPIO (%d)", ret);
		return ret;
	}
	ret = gpio_pin_configure_dt(&config->cled, GPIO_OUTPUT);
	if (ret < 0) {
		LOG_ERR("Could not configure C GPIO (%d)", ret);
		return ret;
	}
	ret = gpio_pin_configure_dt(&config->dled, GPIO_OUTPUT);
	if (ret < 0) {
		LOG_ERR("Could not configure D GPIO (%d)", ret);
		return ret;
	}

	return 0;
}

static const struct steppercontroller_api steppercontroller_api = {
	.step = &step_impl,
};

#define STEPPERCONTROLLER_INIT(i)						       \
	static struct steppercontroller_data steppercontroller_data_##i;	       \
									       \
	static const struct steppercontroller_config steppercontroller_config_##i = {  \
		.aled = GPIO_DT_SPEC_INST_GET(i, a_gpios),		       \
		.bled = GPIO_DT_SPEC_INST_GET(i, b_gpios),		       \
		.cled = GPIO_DT_SPEC_INST_GET(i, c_gpios),		       \
		.dled = GPIO_DT_SPEC_INST_GET(i, d_gpios),		       \
	};								       \
									       \
	DEVICE_DT_INST_DEFINE(i, steppercontroller_init, NULL,		       \
			      &steppercontroller_data_##i,			       \
			      &steppercontroller_config_##i, POST_KERNEL,	       \
			      10, &steppercontroller_api);

DT_INST_FOREACH_STATUS_OKAY(STEPPERCONTROLLER_INIT)