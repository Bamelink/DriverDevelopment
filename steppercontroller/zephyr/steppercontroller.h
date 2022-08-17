/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __STEPPERCONTROLLER_DRIVER_H__
#define __STEPPERCONTROLLER_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/device.h>

typedef enum {
	CLOCKWISE,
	COUNTER_CLOCKWISE
} DIRECTION;

typedef enum {
	FIRST = 0b1000,
	SECOND = 0b0100,
	THIRD = 0b0010,
	FOURTH = 0b0001
} STEPS;

__subsystem struct steppercontroller_api {
	/* This struct has a member called 'print'. 'print' is function
	 * pointer to a function that takes 'struct device *dev' as an
	 * argument and returns 'void'.
	 */
	void (*step)(const struct device *dev, DIRECTION dir, uint32_t steps);
};

__syscall     void        steppercontroller_step(const struct device *dev, DIRECTION dir, uint32_t steps);
static inline void z_impl_steppercontroller_step(const struct device *dev, DIRECTION dir, uint32_t steps)
{
	const struct steppercontroller_api *api = dev->api;

	__ASSERT(api->step, "Callback pointer should not be NULL");

	api->step(dev, dir, steps);
}

#ifdef __cplusplus
}
#endif

#include <syscalls/steppercontroller.h>

#endif /* __STEPPERCONTROLLER_DRIVER_H__ */
