#ifndef _LED_DEVICE_DRIVER_CONSTS_h
#define _LED_DEVICE_DRIVER_CONSTS_h

#include "Device_Driver_Consts.h"

static unsigned int const LED_DEVICE_DRIVER_FIRST_MESSAGE = DEVICE_DRIVER_LAST_MESSAGE + 1002000;
static unsigned int const LED_DEVICE_DRIVER_PIN_LOW = LED_DEVICE_DRIVER_FIRST_MESSAGE + 1;
static unsigned int const LED_DEVICE_DRIVER_PIN_HIGH = LED_DEVICE_DRIVER_FIRST_MESSAGE + 2;
static unsigned int const LED_DEVICE_DRIVER_PIN_BLINK = LED_DEVICE_DRIVER_FIRST_MESSAGE + 3;
static unsigned int const LED_DEVICE_DRIVER_PIN_PULSE = LED_DEVICE_DRIVER_FIRST_MESSAGE + 4;
static unsigned int const LED_DEVICE_DRIVER_LAST_MESSAGE = LED_DEVICE_DRIVER_PIN_PULSE;

#endif