#ifndef _LED_DEVICE_DRIVER_CONSTS_h
#define _LED_DEVICE_DRIVER_CONSTS_h

#include "Device_Driver_Consts.h"
//Intern Communication
static unsigned int const LED_DEVICE_DRIVER_TYPE = DEVICE_DRIVER_LAST_MESSAGE + 1002000;
static unsigned int const LED_DEVICE_DRIVER_LED_ON = LED_DEVICE_DRIVER_TYPE + 1;
static unsigned int const LED_DEVICE_DRIVER_LED_OFF = LED_DEVICE_DRIVER_TYPE + 2;
static unsigned int const LED_DEVICE_DRIVER_LED_BLINK = LED_DEVICE_DRIVER_TYPE + 3;
static unsigned int const LED_DEVICE_DRIVER_LED_PULSE = LED_DEVICE_DRIVER_TYPE + 4;
static unsigned int const LED_DEVICE_DRIVER_LAST_MESSAGE = LED_DEVICE_DRIVER_LED_PULSE;

//Extern Communication
static unsigned int const LED_DEVICE_DRIVER_LED_SET_MODE = LED_DEVICE_DRIVER_TYPE + 101;
static unsigned int const LED_DEVICE_DRIVER_LED_SET_DELAY = LED_DEVICE_DRIVER_TYPE + 102;
static unsigned int const LED_DEVICE_DRIVER_LED_SET_PULSE_COUNT = LED_DEVICE_DRIVER_TYPE + 103;

#endif