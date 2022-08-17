/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT jan_ledcontroller

#include "ledcontroller.h"
#include <zephyr/types.h>
#include <zephyr/syscall_handler.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ledcontroller, CONFIG_GPIO_LOG_LEVEL);


struct ledcontroller_data {
	uint8_t decimal;
};

struct ledcontroller_config {
	struct gpio_dt_spec aled;
	struct gpio_dt_spec bled;
	struct gpio_dt_spec cled;
	struct gpio_dt_spec dled;
};

static void show_binary_impl(const struct device *dev, uint8_t decimal)
{
	printk("show_binary_impl %d\n", decimal);
	const struct ledcontroller_config *config = dev->config;
	struct ledcontroller_data *data = dev->data;
	data->decimal = decimal;

	printk("Received decimal %d\n", decimal);

	if ((decimal > 15) || (decimal < 0)) {
		LOG_ERR("Only 4 LEDs are available, which is 15 in decimal, make sure your number is >=0 and <=15!");
	}
	uint8_t binary[4];
	if(decimal == 0) {
		binary[0] = 0;
		binary[1] = 0;
		binary[2] = 0;
		binary[3] = 0;
	}
	else {
		for(uint8_t i=0; decimal>0; i++)
		{    
			binary[i] = decimal%2;    
			decimal = decimal/2;    
		}
	}
	printk("Binary number is: %d%d%d%d\n", binary[3], binary[2], binary[1], binary[0]);
	gpio_pin_set_dt(&config->aled, binary[3]);
	gpio_pin_set_dt(&config->bled, binary[2]);
	gpio_pin_set_dt(&config->cled, binary[1]);
	gpio_pin_set_dt(&config->dled, binary[0]);
}

#ifdef CONFIG_USERSPACE
static inline void z_vrfy_ledcontroller_show_binary(const struct device *dev, uint8_t decimal)
{
	Z_OOPS(Z_SYSCALL_DRIVER_LEDCONTROLLER(dev, show_binary));

	z_impl_ledcontroller_show_binary(dev, decimal);
}
#include <syscalls/ledcontroller_show_binary_mrsh.c>
#endif /* CONFIG_USERSPACE */


static int ledcontroller_init(const struct device *dev)
{
	printk("ledcontroller_init\n");
	const struct ledcontroller_config *config = dev->config;
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

static const struct ledcontroller_api ledcontroller_api = {
	.show_binary = &show_binary_impl,
};

#define LEDCONTROLLER_INIT(i)						       \
	static struct ledcontroller_data ledcontroller_data_##i;	       \
									       \
	static const struct ledcontroller_config ledcontroller_config_##i = {  \
		.aled = GPIO_DT_SPEC_INST_GET(i, a_gpios),		       \
		.bled = GPIO_DT_SPEC_INST_GET(i, b_gpios),		       \
		.cled = GPIO_DT_SPEC_INST_GET(i, c_gpios),		       \
		.dled = GPIO_DT_SPEC_INST_GET(i, d_gpios),		       \
	};								       \
									       \
	DEVICE_DT_INST_DEFINE(i, ledcontroller_init, NULL,		       \
			      &ledcontroller_data_##i,			       \
			      &ledcontroller_config_##i, POST_KERNEL,	       \
			      10, &ledcontroller_api);

DT_INST_FOREACH_STATUS_OKAY(LEDCONTROLLER_INIT)