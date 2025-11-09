/*
 * NPLL - Hollywood/Latte Hardware - GPIO
 *
 * Copyright (C) 2025 Techflash
 */

#include <stdio.h>
#include <npll/types.h>
#include <npll/drivers.h>
#include <npll/hollywood/gpio.h>

static REGISTER_DRIVER(gpioDrv);

static u32 prevIn = 0;
/* TODO: translate power/eject (and check PI for reset on GCN/Wii) into inputs */
static void gpioCallback(void) {
	u32 in = HW_GPIOB_IN;

	if (in & GPIO_POWER && !(prevIn & GPIO_POWER))
		puts("GPIO: Power button pressed");
	if (in & GPIO_EJECT_BTN && !(prevIn & GPIO_EJECT_BTN))
		puts("GPIO: Eject button pressed");
	if (in & GPIO_SLOT_IN && !(prevIn & GPIO_SLOT_IN))
		puts("GPIO: Disc inserted");
	else if (!(in & GPIO_SLOT_IN) && prevIn & GPIO_SLOT_IN)
		puts("GPIO: Disc removed");

	prevIn = in;
}

static void gpioInit(void) {
	/* Broadway owns all */
	HW_GPIO_OWNER = 0xffffffff;

	/* all enabled */
	HW_GPIO_ENABLE = 0xffffffff;

	/* set proper directions */
	HW_GPIOB_DIR = ~(GPIO_POWER | GPIO_EJECT_BTN | GPIO_SLOT_IN | GPIO_EEP_MISO | GPIO_AVE_SDA);
	HW_GPIO_DIR  = HW_GPIOB_DIR;

	/* set up outputs properly */
	HW_GPIOB_OUT = GPIO_DC_DC | GPIO_FAN | GPIO_SENSOR_BAR | GPIO_SLOT_LED;
	HW_GPIO_OUT  = HW_GPIOB_OUT;

	/* register our callback to check GPIOs */
	D_AddCallback(gpioCallback);

	/* we're all good */
	gpioDrv.state = DRIVER_STATE_READY;
}

static void gpioCleanup(void) {
	D_RemoveCallback(gpioCallback);
	gpioDrv.state = DRIVER_STATE_NOT_READY;
}

static REGISTER_DRIVER(gpioDrv) = {
	.name = "Hollywood/Latte GPIO",
	.mask = DRIVER_ALLOW_WII | DRIVER_ALLOW_WIIU,
	.state = DRIVER_STATE_NOT_READY,
	.type = DRIVER_TYPE_OTHER,
	.init = gpioInit,
	.cleanup = gpioCleanup
};
