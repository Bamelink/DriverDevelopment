/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __HELLO_WORLD_DRIVER_H__
#define __HELLO_WORLD_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/device.h>

/*
 * This 'Hello World' driver has a 'print' syscall that prints the
 * famous 'Hello World!' string.
 *
 * The string is formatted with some internal driver data to
 * demonstrate that drivers are initialized during the boot process.
 *
 * The driver exists to demonstrate (and test) custom drivers that are
 * maintained outside of Zephyr.
 */
__subsystem struct ledcontroller_api {
	/* This struct has a member called 'print'. 'print' is function
	 * pointer to a function that takes 'struct device *dev' as an
	 * argument and returns 'void'.
	 */
	void (*show_binary)(const struct device *dev, uint8_t decimal);
};

__syscall     void        ledcontroller_show_binary(const struct device *dev, uint8_t decimal);
static inline void z_impl_ledcontroller_show_binary(const struct device *dev, uint8_t decimal)
{
	printk("z_impl_ledcontroller_show_binary\n");
	const struct ledcontroller_api *api = dev->api;

	__ASSERT(api->show_binary, "Callback pointer should not be NULL");

	api->show_binary(dev, decimal);
}

#ifdef __cplusplus
}
#endif

#include <syscalls/ledcontroller.h>

#endif /* __HELLO_WORLD_DRIVER_H__ */
