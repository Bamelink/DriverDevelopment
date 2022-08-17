/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ledcontroller.h"
#include <stdio.h>
#include <zephyr/zephyr.h>

const struct device *dev;

static void user_entry(void *p1, void *p2, void *p3)
{
	printk("user_entry\n");
	uint8_t decimal = 0;
	while (1)
	{
		if(decimal > 15) {
			decimal = 0;
		}
		ledcontroller_show_binary(dev, decimal);
		decimal += 1;
		k_msleep(1000);
	}
}

void main(void)
{
	printk("Hello World from the app!\n");

	dev = DEVICE_DT_GET(DT_NODELABEL(ledcontroller0));

	__ASSERT(dev, "Failed to get device binding");

	printk("device is %p, name is %s\n", dev, dev->name);

	k_object_access_grant(dev, k_current_get());
	k_thread_user_mode_enter(user_entry, NULL, NULL, NULL);
}
