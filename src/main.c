/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//#include "ledcontroller.h"
#include "steppercontroller.h"
#include <stdio.h>
#include <zephyr/zephyr.h>

const struct device *dev;

static void user_entry(void *p1, void *p2, void *p3)
{
	while (1)
	{
		steppercontroller_step(dev, CLOCKWISE, 200);
		steppercontroller_step(dev, COUNTER_CLOCKWISE, 200);
	}
}

void main(void)
{
	printk("Hello World from the app!\n");

	//dev = DEVICE_DT_GET(DT_NODELABEL(ledcontroller0));
	dev = DEVICE_DT_GET(DT_NODELABEL(steppercontroller0));

	__ASSERT(dev, "Failed to get device binding");

	printk("device is %p, name is %s\n", dev, dev->name);

	k_object_access_grant(dev, k_current_get());
	k_thread_user_mode_enter(user_entry, NULL, NULL, NULL);
}
